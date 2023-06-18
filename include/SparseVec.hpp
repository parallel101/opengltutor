#pragma once

#include <unordered_map>
#include <utility>
#include <memory>
#include <cstdint>
#include <concepts>
#include <vector>
#include <cassert>
#include <typeindex>
#include <variant>

template <class T, std::size_t N>
union UninitArray {
    T inner[N];

    constexpr UninitArray() {}
    constexpr ~UninitArray() {}

    [[nodiscard]] constexpr T &operator[](std::size_t i) noexcept {
        assert(i < N);
        return inner[i];
    }

    [[nodiscard]] constexpr T const &operator[](std::size_t i) const noexcept {
        assert(i < N);
        return inner[i];
    }

    template <class ...Ts>
    constexpr T *construct_at(std::size_t i, Ts &&...ts)
        noexcept(noexcept(std::construct_at(inner + i, std::forward<Ts>(ts)...)))
    {
        return std::construct_at(inner + i, std::forward<Ts>(ts)...);
    }

    constexpr void destroy_at(std::size_t i) noexcept {
        return std::destroy_at(inner + i);
    }
};

template <class T>
struct AutoConstructPtr : std::unique_ptr<T> {
    using std::unique_ptr<T>::unique_ptr;

    AutoConstructPtr() noexcept(noexcept(T()))
    : std::unique_ptr<T>(std::make_unique<T>())
    {}
};

template <class T, class Id = std::size_t, class DenseId = std::size_t, class Alloc = std::allocator<T>>
struct SparseVec {
    struct SparseBlock {
        static const std::size_t N = 10;
        alignas(64) DenseId dids[1 << N];
        alignas(64) std::uint8_t bits[1 << (N - 3)];
        SparseBlock() noexcept : bits{} {}
    };

    struct DenseBlock {
        static const std::size_t N = 10;
        alignas(64) Id inds[1 << N];
        alignas(std::max((std::size_t)64, alignof(T))) UninitArray<T, (1 << N)> vals;
        DenseBlock() noexcept {}
    };
    std::vector<AutoConstructPtr<SparseBlock>, typename std::allocator_traits<Alloc>::template rebind_alloc<AutoConstructPtr<SparseBlock>>> m_sparse;
    std::vector<AutoConstructPtr<DenseBlock>, typename std::allocator_traits<Alloc>::template rebind_alloc<AutoConstructPtr<DenseBlock>>> m_dense;
    DenseId m_densize;
    Id m_spsize;

    SparseVec() noexcept : m_densize(0), m_spsize(0) {}

    SparseVec(SparseVec &&that) noexcept
    : m_sparse(std::move(that.m_sparse))
    , m_dense(std::move(that.m_dense))
    , m_densize(std::exchange(that.m_densize, 0))
    , m_spsize(std::exchange(that.m_spsize, 0))
    {}

    SparseVec &operator=(SparseVec &&that) noexcept {
        if (this != &that) [[likely]] {
            m_sparse = std::move(that.m_sparse);
            m_dense = std::move(that.m_dense);
            m_densize = std::exchange(that.m_densize, 0);
            m_spsize = std::exchange(that.m_spsize, 0);
        }
        return *this;
    }

    SparseVec(SparseVec const &that) noexcept
    : m_sparse(that.m_sparse)
    , m_dense(that.m_dense)
    , m_densize(that.m_densize)
    , m_spsize(that.m_spsize)
    {}

    SparseVec &operator=(SparseVec const &that) noexcept {
        if (this != &that) [[likely]] {
            m_sparse = that.m_sparse;
            m_dense = that.m_dense;
            m_densize = that.m_densize;
            m_spsize = that.m_spsize;
        }
        return *this;
    }

    void swap(SparseVec &that) noexcept {
        if (this != &that) [[likely]] {
            std::swap(m_sparse, that.m_sparse);
            std::swap(m_dense, that.m_dense);
            std::swap(m_densize, that.m_densize);
            std::swap(m_spsize, that.m_spsize);
        }
    }

    void _sparse_emplace(Id id, DenseId did) noexcept {
        std::size_t blknr = id >> SparseBlock::N;
        std::size_t blkoff = id & ((1 << SparseBlock::N) - 1);
        if (m_sparse.size() <= blknr) {
            m_sparse.resize(blknr + 1);
        }
        SparseBlock &blk = *m_sparse[blknr];
        blk.dids[blkoff] = did;
        blk.bits[blkoff >> 3] |= (1 << (blkoff & 7));
        ++m_spsize;
    }

    bool _sparse_erase(Id id, DenseId &did) noexcept {
        std::size_t blknr = (std::size_t)id >> SparseBlock::N;
        std::size_t blkoff = (std::size_t)id & ((1 << SparseBlock::N) - 1);
        if (m_sparse.size() <= blknr) {
            return false;
        }
        SparseBlock &blk = *m_sparse[blknr];
        blk.dids[blkoff] = did;
        blk.bits[blkoff >> 3] &= ~(1 << (blkoff & 7));
        --m_spsize;
        return true;
    }

    bool _sparse_read(Id id, DenseId &did) noexcept {
        std::size_t blknr = (std::size_t)id >> SparseBlock::N;
        std::size_t blkoff = (std::size_t)id & ((1 << SparseBlock::N) - 1);
        if (m_sparse.size() <= blknr) {
            return false;
        }
        SparseBlock &blk = *m_sparse[blknr];
        did = blk.dids[blkoff];
        return blk.bits[blkoff >> 3] & (1 << (blkoff & 7));
    }

    void _sparse_update(Id id, DenseId did) noexcept {
        std::size_t blknr = (std::size_t)id >> SparseBlock::N;
        std::size_t blkoff = (std::size_t)id & ((1 << SparseBlock::N) - 1);
        SparseBlock &blk = *m_sparse[blknr];
        blk.dids[blkoff] = did;
    }

    template <class ...Args>
    T *_dense_emplace_back(Id id, Args &&...args) noexcept {
        std::size_t blknr = (std::size_t)m_densize >> DenseBlock::N;
        std::size_t blkoff = (std::size_t)m_densize & ((1 << DenseBlock::N) - 1);
        if (m_dense.size() <= blknr) {
            m_dense.resize(blknr + 1);
        }
        DenseBlock &blk = *m_dense[blknr];
        blk.inds[blkoff] = id;
        T *ret = blk.vals.construct_at(blkoff, std::forward<Args>(args)...);
        ++m_densize;
        return ret;
    }

    void _dense_pop_back() noexcept {
        std::size_t bblknr = (std::size_t)m_densize >> DenseBlock::N;
        std::size_t bblkoff = (std::size_t)m_densize & ((1 << DenseBlock::N) - 1);
        DenseBlock &bblk = *m_dense[bblknr];
        bblk.vals.destroy_at(bblkoff);
        --m_densize;
    }

    Id _dense_swap_erase_back(Id id) noexcept {
        std::size_t blknr = (std::size_t)id >> DenseBlock::N;
        std::size_t blkoff = (std::size_t)id & ((1 << DenseBlock::N) - 1);
        DenseBlock &blk = *m_dense[blknr];
        std::size_t bblknr = (std::size_t)m_densize >> DenseBlock::N;
        std::size_t bblkoff = (std::size_t)m_densize & ((1 << DenseBlock::N) - 1);
        DenseBlock &bblk = *m_dense[bblknr];
        blk.vals.destroy_at(blkoff);
        blk.vals.construct_at(blkoff, std::move(bblk.vals[bblkoff]));
        bblk.vals.destroy_at(bblkoff);
        Id indb = bblk.inds[bblkoff];
        blk.inds[blkoff] = indb;
        --m_densize;
        return indb;
    }

    T &_dense_at(Id id) noexcept {
        std::size_t blknr = (std::size_t)id >> DenseBlock::N;
        std::size_t blkoff = (std::size_t)id & ((1 << DenseBlock::N) - 1);
        DenseBlock &blk = *m_dense[blknr];
        return blk.vals[blkoff];
    }

    T const &_dense_at(Id id) const noexcept {
        std::size_t blknr = (std::size_t)id >> DenseBlock::N;
        std::size_t blkoff = (std::size_t)id & ((1 << DenseBlock::N) - 1);
        DenseBlock &blk = *m_dense[blknr];
        return blk.vals[blkoff];
    }

    Id push_back(T &&val) noexcept {
        return emplace_back(std::move(val)).first;
    }

    Id push_back(T const &val) noexcept {
        return emplace_back(val).first;
    }

    template <class ...Args>
    std::pair<Id, T *> emplace_back(Args &&...args) noexcept {
        Id id = m_spsize;
        DenseId did = m_densize;
        _sparse_emplace(id, did);
        return {id, _dense_emplace_back(id, std::forward<Args>(args)...)};
    }

    [[nodiscard]] T &operator[](Id id) noexcept {
        T *p = at(id);
        assert(p);
        return *p;
    }

    [[nodiscard]] T const &operator[](Id id) const noexcept {
        T const *p = at(id);
        assert(p);
        return *p;
    }

    [[nodiscard]] T *at(Id id) noexcept {
        DenseId did;
        if (!_sparse_read(id, did)) {
            return nullptr;
        }
        return &_dense_at(did);
    }

    [[nodiscard]] T const *at(Id id) const noexcept {
        DenseId did;
        if (!_sparse_read(id, did)) {
            return nullptr;
        }
        return &_dense_at(did);
    }

    [[nodiscard]] bool contains(Id id) const noexcept {
        DenseId did;
        return _sparse_read(id, did);
    }

    void pop_back() noexcept {
        DenseId did;
        _sparse_erase(m_densize - 1, did);
        _dense_pop_back();
    }

    bool erase(Id id) noexcept {
        if (id + 1 == m_densize && m_densize != 0) {
            pop_back();
            return true;
        }
        DenseId did;
        if (!_sparse_erase(id, did)) {
            return false;
        }
        Id indb = _dense_swap_erase_back(id);
        _sparse_update(indb, did);
        return true;
    }

    template <class Fn>
    void foreach(Fn &&fn) noexcept {
        return _impl_foreach(*this, std::forward<Fn>(fn));
    }

    template <class Fn>
    void foreach(Fn &&fn) const noexcept {
        return _impl_foreach(*this, std::forward<Fn>(fn));
    }

    template <class Self, class Fn>
    static void _impl_foreach(Self &&self, Fn &&fn) noexcept {
        const std::size_t nblks = (std::size_t)self.m_densize >> DenseBlock::N;
        const std::size_t nrest = self.m_densize - (nblks << DenseBlock::N);
        if (nblks) {
            for (std::size_t blknr = 0; blknr != nblks; blknr++) {
                auto &blk = *self.m_dense[blknr];
                for (std::size_t blkoff = 0; blkoff != (1 << DenseBlock::N); blkoff++) {
                    fn(blk.inds[blkoff], blk.vals[blkoff]);
                }
            }
        }
        if (nrest) {
            auto &bblk = *self.m_dense[nblks];
            for (std::size_t bblkoff = 0; bblkoff != nrest; bblkoff++) {
                fn(bblk.inds[bblkoff], bblk.vals[bblkoff]);
            }
        }
    }

    template <class Fn>
    void foreach_block(Fn &&fn) noexcept {
        return _impl_foreach_block(*this, std::forward<Fn>(fn));
    }

    template <class Fn>
    void foreach_block(Fn &&fn) const noexcept {
        return _impl_foreach_block(*this, std::forward<Fn>(fn));
    }

    template <class Self, class Fn>
    static void _impl_foreach_block(Self &&self, Fn &&fn) noexcept {
        const std::size_t nblks = (std::size_t)self.m_densize >> DenseBlock::N;
        const std::size_t nrest = self.m_densize - (nblks << DenseBlock::N);
        if (nblks) {
            for (std::size_t blknr = 0; blknr != nblks; blknr++) {
                auto &blk = *self.m_dense[blknr];
                fn(blk);
            }
        }
        if (nrest) {
            auto &bblk = *self.m_dense[nblks];
            fn(bblk, nrest);
        }
    }

    DenseBlock &block(std::size_t blknr) noexcept {
        assert(blknr < block_count());
        return *m_dense[blknr];
    }

    DenseBlock const &block(std::size_t blknr) const noexcept {
        assert(blknr < block_count());
        return *m_dense[blknr];
    }

    static constexpr std::size_t block_size() noexcept {
        return DenseBlock::N;
    }

    std::size_t block_count() const noexcept {
        return (std::size_t)m_densize >> DenseBlock::N;
    }

    std::size_t last_block_size() const noexcept {
        return m_densize - (block_count() << DenseBlock::N);
    }

    [[nodiscard]] DenseId size() const noexcept {
        return m_densize;
    }

    [[nodiscard]] Id max_id() const noexcept {
        return m_spsize;
    }
};

template <class Id = std::size_t, class Alloc = std::allocator<std::byte>>
struct BlobVec {
    union DenseBlock {
        static const std::size_t N = 8;
        alignas(1 << N) std::byte uninit[1 << N];
        DenseBlock() noexcept {}
        ~DenseBlock() noexcept {}
    };

    using allocator = typename std::allocator_traits<Alloc>::template rebind_alloc<DenseBlock>;
    DenseBlock *m_blks;
    std::size_t m_size;
    std::size_t m_capacity;
    void (*m_dtor)(void *, std::size_t) noexcept;
    void (*m_movctor)(void *, void const *, std::size_t) noexcept;
    void (*m_deflctor)(void *, std::size_t) noexcept;
    std::size_t m_elmsize;
    std::type_info const *m_type;
    [[no_unique_address]] allocator m_alloc;

    template <class T>
    static void _destructor_for(void *ptr, std::size_t n) noexcept {
        T *base = reinterpret_cast<T *>(ptr);
        for (std::size_t i = 0; i < n; i++) {
            std::destroy_at(base + i);
        }
    }

    template <class T>
    static void _move_constructor_for(void *ptr, void const *srcptr, std::size_t n) noexcept {
        T *base = reinterpret_cast<T *>(ptr);
        T const *srcbase = reinterpret_cast<T const *>(srcptr);
        for (std::size_t i = 0; i < n; i++) {
            std::construct_at(base + i, std::move(srcbase[i]));
        }
    }

    template <class T>
    static void _default_constructor_for(void *ptr, std::size_t n) noexcept {
        T *base = reinterpret_cast<T *>(ptr);
        for (std::size_t i = 0; i < n; i++) {
            std::construct_at(base + i);
        }
    }

    BlobVec(BlobVec &&that) noexcept
        : m_blks(std::move(that.m_blks))
        , m_size(std::exchange(that.m_size, 0))
        , m_capacity(std::exchange(that.m_capacity, 0))
        , m_dtor(std::exchange(that.m_dtor, nullptr))
        , m_movctor(std::exchange(that.m_movctor, nullptr))
        , m_deflctor(std::exchange(that.m_deflctor, nullptr))
        , m_elmsize(std::exchange(that.m_elmsize, nullptr))
        , m_type(std::exchange(that.m_type, nullptr))
    {
    }

    BlobVec &operator=(BlobVec &&that) noexcept {
        if (this != &that) [[likely]] {
            m_blks = std::move(that.m_blks);
            m_size = std::exchange(that.m_size, 0);
            m_capacity = std::exchange(that.m_capacity, 0);
            m_dtor = std::exchange(that.m_dtor, nullptr);
            m_movctor = std::exchange(that.m_movctor, nullptr);
            m_deflctor = std::exchange(that.m_deflctor, nullptr);
            m_elmsize = std::exchange(that.m_elmsize, nullptr);
            m_type = std::exchange(that.m_type, nullptr);
        }
        return *this;
    }

    BlobVec() noexcept
        : m_blks()
        , m_size(0)
        , m_capacity(0)
        , m_dtor(nullptr)
        , m_movctor(nullptr)
        , m_deflctor(nullptr)
        , m_elmsize(0)
        , m_type(nullptr)
    {
    }

    DenseBlock *_allocate(std::size_t cap) {
        return std::allocator_traits<allocator>::allocate(m_alloc, cap);
    }

    void _deallocate(DenseBlock *ptr, std::size_t cap) {
        std::allocator_traits<allocator>::deallocate(m_alloc, ptr, cap);
    }

    ~BlobVec() noexcept {
        deinitialize_type();
    }

    void clear() noexcept {
        assert(m_dtor);
        m_dtor(m_blks, m_size);
        m_size = 0;
    }

    void deinitialize_type() noexcept {
        if (m_dtor) {
            clear();
            _deallocate(m_blks, m_capacity);
        } else {
            assert(!m_capacity);
        }
        m_blks = nullptr;
        m_capacity = 0;
        m_dtor = nullptr;
        m_movctor = nullptr;
        m_deflctor = nullptr;
        m_dtor = nullptr;
        m_elmsize = 0;
        m_type = nullptr;
    }

    template <class T>
    void initialize_type() noexcept {
        assert(!m_type);
        assert(!m_dtor);
        assert(!m_elmsize);
        static_assert(alignof(T) <= alignof(DenseBlock));
        m_dtor = _destructor_for<T>;
        m_movctor = _move_constructor_for<T>;
        m_deflctor = _default_constructor_for<T>;
        m_elmsize = sizeof(T);
        m_type = &typeid(T);
    }

    template <class T>
    explicit BlobVec(std::in_place_type_t<T>) noexcept
        : m_blks()
        , m_size(0)
        , m_capacity(0)
        , m_dtor(_destructor_for<T>)
        , m_movctor(_move_constructor_for<T>)
        , m_deflctor(_default_constructor_for<T>)
        , m_elmsize(sizeof(T))
        , m_type(&typeid(T))
    {
        static_assert(alignof(T) <= alignof(DenseBlock));
    }

    template <class T, class ...Args>
        requires (std::constructible_from<T, Args const &...>)
    void resize(std::size_t n, Args const &...args) noexcept {
        assert(&typeid(T) == m_type);
        std::size_t old_size = m_size;
        _grow_at_least<T>(n);
        m_size = n;
        if (old_size < m_size) {
            T *base = reinterpret_cast<T *>(m_blks);
            for (std::size_t i = old_size; i < m_size; i++) {
                std::construct_at(base + i, args...);
            }
        }
    }

    void resize(std::size_t n) noexcept {
        std::size_t old_size = m_size;
        _grow_at_least(n);
        m_size = n;
        if (old_size < m_size) {
            m_deflctor(reinterpret_cast<std::byte *>(m_blks) + old_size * m_elmsize, m_size - old_size);
        }
    }

    template <class T>
    T &push_back(std::type_identity<T> &&val) noexcept {
        return emplace_back(std::move(val));
    }

    template <class T>
    T &push_back(std::type_identity<T> const &val) noexcept {
        return emplace_back(val);
    }

    void _grow_at_least(std::size_t needed) noexcept {
        if (m_capacity < needed) {
            DenseBlock *new_blks = _allocate(std::max(m_capacity << 1, needed));
            m_movctor(new_blks, m_blks, m_size);
            _deallocate(m_blks, m_capacity);
            m_blks = new_blks;
            m_capacity = needed;
        }
    }

    template <class T>
    void _grow_at_least(std::size_t needed) noexcept {
        if (m_capacity < needed) {
            DenseBlock *new_blks = _allocate(std::max(m_capacity << 1, needed));
            T *new_ptr = reinterpret_cast<T *>(new_blks);
            T *old_ptr = reinterpret_cast<T *>(m_blks);
            for (std::size_t i = 0; i < m_size; i++) {
                std::construct_at(new_ptr + i, std::move(old_ptr[i]));
            }
            _deallocate(m_blks, m_capacity);
            m_blks = new_blks;
            m_capacity = needed;
        }
    }

    template <class T, class ...Args>
        requires (std::constructible_from<T, Args...>)
    T &emplace_back(Args &&...args) noexcept {
        assert(&typeid(T) == m_type);
        std::size_t needed = (m_size * sizeof(T)) >> DenseBlock::N;
        _grow_at_least<T>(needed);
        T *ptr = reinterpret_cast<T *>(m_blks) + m_size;
        std::construct_at(ptr, std::forward<Args>(args)...);
        return *ptr;
    }

    void emplace_back() noexcept {
        std::size_t needed = (m_size * m_elmsize) >> DenseBlock::N;
        _grow_at_least(needed);
        auto ptr = reinterpret_cast<std::byte *>(m_blks) + m_size * m_elmsize;
        m_deflctor(ptr, 1);
    }

    template <class T>
    T const *at(std::size_t i) const noexcept {
        assert(&typeid(T) == m_type);
        if (i < m_size) [[unlikely]] {
            return nullptr;
        }
        std::size_t offset = i * sizeof(T);
        T const *ptr = reinterpret_cast<T const *>(m_blks + offset);
        return *ptr;
    }

    template <class T>
    T *at(std::size_t i) noexcept {
        assert(&typeid(T) == m_type);
        if (i < m_size) [[unlikely]] {
            return nullptr;
        }
        std::size_t offset = i * sizeof(T);
        T *ptr = reinterpret_cast<T *>(m_blks + offset);
        return *ptr;
    }

    template <class T>
    T const &get(std::size_t i) const noexcept {
        assert(&typeid(T) == m_type);
        assert(i < m_size);
        std::size_t offset = (m_size - 1) * sizeof(T);
        T const *ptr = reinterpret_cast<T const *>(m_blks + offset);
        return *ptr;
    }

    template <class T>
    T &get(Id i) noexcept {
        auto i_ = (std::size_t)i;
        assert(&typeid(T) == m_type);
        assert(i_ < m_size);
        std::size_t offset = (m_size - 1) * sizeof(T);
        T *ptr = reinterpret_cast<T *>(m_blks + offset);
        return *ptr;
    }

    template <class T>
    T const &front() const noexcept {
        assert(&typeid(T) == m_type);
        assert(m_size);
        T const *ptr = reinterpret_cast<T const *>(m_blks);
        return *ptr;
    }

    template <class T>
    T &front() noexcept {
        assert(&typeid(T) == m_type);
        assert(m_size);
        T *ptr = reinterpret_cast<T *>(m_blks);
        return *ptr;
    }

    template <class T>
    T const &back() const noexcept {
        assert(&typeid(T) == m_type);
        assert(m_size);
        std::size_t offset = (m_size - 1) * sizeof(T);
        T const *ptr = reinterpret_cast<T const *>(m_blks + offset);
        return *ptr;
    }

    template <class T>
    T &back() noexcept {
        assert(&typeid(T) == m_type);
        assert(m_size);
        std::size_t offset = (m_size - 1) * sizeof(T);
        T *ptr = reinterpret_cast<T *>(m_blks + offset);
        return *ptr;
    }

    void pop_back() noexcept {
        assert(m_dtor);
        assert(m_size);
        std::size_t offset = (m_size - 1) * m_elmsize;
        m_dtor(reinterpret_cast<std::byte *>(m_blks) + offset, 1);
    }

    void erase(Id i, Id n = 1) noexcept {
        auto i_ = (std::size_t)i;
        auto n_ = (std::size_t)n;
        assert(m_dtor);
        assert(i_ < m_size && i_ + n_ <= m_size);
        std::size_t offset = i_ * m_elmsize;
        m_dtor(reinterpret_cast<std::byte *>(m_blks) + offset, n_);
    }

    Id size() noexcept {
        return (Id)m_size;
    }

    void const *data() const noexcept {
        return m_blks;
    }

    void *data() noexcept {
        return m_blks;
    }

    template <class T>
    T const *data() const noexcept {
        return reinterpret_cast<T const *>(m_blks);
    }

    template <class T>
    T *data() noexcept {
        return reinterpret_cast<T *>(m_blks);
    }

    template <class T>
    struct _Range {
        T *m_begin;
        T *m_end;

        constexpr T *begin() const {
            return m_begin;
        }

        constexpr T *end() const {
            return m_end;
        }
    };

    template <class T>
    _Range<T> range() noexcept {
        T *base = reinterpret_cast<T *>(m_blks);
        return {base, base + m_size};
    }

    template <class T>
    _Range<T const> range() const noexcept {
        T *base = reinterpret_cast<T const *>(m_blks);
        return {base, base + m_size};
    }

    template <class T>
    _Range<T const> crange() const noexcept {
        T *base = reinterpret_cast<T const *>(m_blks);
        return {base, base + m_size};
    }
};

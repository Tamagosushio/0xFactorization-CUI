#pragma once

// class Tを最大MAXSIZE個確保可能なPoolAllocatorを実装してください
template<class T, size_t MAXSIZE> class PoolAllocator
{
public:
	// コンストラクタ
	PoolAllocator() {
		// TODO: 必要に応じて実装してください
        for (size_t i = 0; i < MAXSIZE - 1; i++) {
            m_pool[i].next = &m_pool[i + 1];
        }
        m_pool[MAXSIZE - 1].next = nullptr;
        m_free_list_head = &m_pool[0];
	}

	// デストラクタ
	~PoolAllocator() {
		// TODO: 必要に応じて実装してください

	}

	// 確保できない場合はnullptrを返す事。
	T* Alloc() {
		// TODO: 実装してください
        if (m_free_list_head == nullptr) {
            return nullptr;
        }
        ElementType* block = m_free_list_head;
        m_free_list_head = block->next;
        //new (block)T();
        return reinterpret_cast<T*>(block);
	}

	// Free(nullptr)で誤動作しないようにする事。
	void Free(T* addr) {
		// TODO: 実装してください
        if (addr == nullptr) {
            return;
        }
        addr->~T();
        ElementType* block = reinterpret_cast<ElementType*>(addr);
        block->next = m_free_list_head;
        m_free_list_head = block;
	}

private:
	// TODO: 実装してください
    union ElementType {
        T obj;
        ElementType* next;
        ElementType() {};
        ~ElementType() {};
    };
    ElementType m_pool[MAXSIZE];
    ElementType* m_free_list_head;
};

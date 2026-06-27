#ifndef INDEX_BUFFER_H
#define INDEX_BUFFER_H

namespace Poke
{
    class IndexBuffer
    {
    public:
        IndexBuffer(const unsigned int *data, unsigned int count);
        ~IndexBuffer();

        void Bind() const;
        void Unbind() const;

        inline unsigned int GetCount() const { return m_count; }

    private:
        unsigned int m_rendererID;
        unsigned int m_count;
    };
}

#endif
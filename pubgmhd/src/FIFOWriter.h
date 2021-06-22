#ifndef FIFO_WRITER_H
#define FIFO_WRITER_H

#define BUF_SIZE 1024

class FIFOWriter{
private:
    const char *m_path;
    int m_fd;
public:
    FIFOWriter(const char *path);
    int init_fifo();
    int write_data(char *data, int size);

};

#endif
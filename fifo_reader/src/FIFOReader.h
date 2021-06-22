#ifndef FIFO_READER_H
#define FIFO_READER_H

#define BUF_SIZE 1024

class FIFOReader{
private:
    const char *m_path;
    int m_fd;
public:
    FIFOReader(const char *path);
    int init_fifo();
    int read_data();

};

#endif
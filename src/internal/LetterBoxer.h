#ifndef LETTERBOXER_H
#define LETTERBOXER_H


class LetterBoxer
{
public:
    LetterBoxer(int bufferWidth, int bufferHeight, int embedWidth, int embedHeight, int channels);
    ~LetterBoxer();

    void letterbox(unsigned char *pylonBuffer, float *darknetBuffer);

private:
    int pylonWidth;
    int pylonHeight;
    int bufferSize;
    int bufferChannels;
    int darknetWidth;
    int darknetHeight;
    int darknetSize;
    double resizeFactor;
    int embeddedImageSize;
    int embeddedImageHeight;
    int blockSize;
    int numBlocks;
    int padding;
    int smallpadding;

    unsigned char *charBuffer;
    float *floatBuffer;
    float *smallBuffer;
};

#endif // LETTERBOXER_H

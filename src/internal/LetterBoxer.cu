#include "LetterBoxer.h"
#include <cuda_runtime.h>
#include <iostream>

LetterBoxer::LetterBoxer(int bufferWidth, int bufferHeight, int embedWidth, int embedHeight, int channels):
    pylonWidth(bufferWidth),
    pylonHeight(bufferHeight),
    bufferSize(bufferWidth * bufferHeight * channels),
    bufferChannels(channels),
    darknetWidth(embedWidth),
    darknetHeight(embedHeight),
    darknetSize(darknetWidth * darknetHeight * channels),
    resizeFactor(double(pylonWidth) / double(darknetWidth)),
    embeddedImageSize(darknetWidth * darknetHeight),
    embeddedImageHeight(int(pylonHeight / resizeFactor)),
    blockSize(256),
    numBlocks((blockSize + bufferSize - 1) / blockSize),
    padding(((darknetHeight - embeddedImageHeight) / 2) * pylonWidth),
    smallpadding(((darknetHeight - embeddedImageHeight) / 2) * darknetWidth)
{
    cudaMallocManaged(&charBuffer, bufferSize * sizeof(unsigned char));
    cudaMallocManaged(&floatBuffer, bufferSize * sizeof(float));
    cudaMallocManaged(&smallBuffer, darknetSize * sizeof(float));
}

LetterBoxer::~LetterBoxer()
{
    cudaDeviceSynchronize();
    cudaFree(charBuffer);
    cudaFree(floatBuffer);
    cudaFree(smallBuffer);
}

__global__ void initialFloat(float *buffer, int size)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (int i = index; i < size; i += stride){
        buffer[i] = 0.5f;
    }
}

__global__ void embedLetterbox(float *bufferOut, unsigned char *bufferIn, int embeddedImageSize, int smallsize, int bufferSize, int channels, int embeddedImageWidth, int bufferWidth, double factor, int padding)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (int i = index; i < embeddedImageSize * 3; i += stride){
        int j = i % embeddedImageSize;
        bufferOut[(i / embeddedImageSize) * smallsize + j + padding] =
                bufferIn[(i / embeddedImageSize) * (bufferSize / 3) + int(double(j / embeddedImageWidth) * factor) * bufferWidth + int(double(j % embeddedImageWidth) * factor)] / 255.0;
    }
}

void LetterBoxer::letterbox(unsigned char *pylonBuffer, float *darknetBuffer)
{
    cudaMemcpy(charBuffer, pylonBuffer, bufferSize * sizeof(unsigned char), cudaMemcpyHostToDevice);

    initialFloat<<<numBlocks, blockSize>>>(smallBuffer, darknetSize);
    embedLetterbox<<<numBlocks, blockSize>>>(smallBuffer, charBuffer, embeddedImageHeight * darknetWidth, embeddedImageSize, bufferSize, bufferChannels, darknetWidth, pylonWidth, resizeFactor, smallpadding);

    cudaDeviceSynchronize();

    cudaMemcpy(darknetBuffer, smallBuffer, darknetSize * sizeof(float), cudaMemcpyDeviceToHost);
}

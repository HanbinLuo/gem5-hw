#ifndef UART16550_H
#define UART16550_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#define LOGF(...) vLogPrintfLn(__VA_ARGS__) /* 带 core 前缀 */
#define LOGF0(...) vUartPrintf(__VA_ARGS__) /* 不带前缀 */

void prvUartInit(void);
void prvUartPutRaw(char c);
void prvUartWrite(const char* pcData, size_t xLength);
size_t prvWriteString(char* pcDest, const char* pcSrc);
size_t prvWriteDecimal(char* pcDest, uint32_t ulValue);
size_t prvWriteHex(char* pcDest, uint32_t ulValue);
// void vDemoLogDecimal(const char* pcLabel, uint32_t ulValue);
// void vDemoLogHexPair(const char* pcLabel, uint32_t ulFirst, uint32_t
// ulSecond); void vDemoLogString(const char* pcMsg);
void vUartPrintf(const char* pcFmt, ...);
void vLogPrintfLn(const char* pcFmt, ...);

#endif /* UART16550_H */
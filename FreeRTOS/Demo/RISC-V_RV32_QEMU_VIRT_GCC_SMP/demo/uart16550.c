#include "uart16550.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "FreeRTOS.h"
#include "portmacro.h"

#define UART0_BASE 0x10000000UL
#define UART_REG_DLL 0x00
#define UART_REG_DLM 0x01
#define UART_REG_FCR 0x02
#define UART_REG_LCR 0x03
#define UART_REG_MCR 0x04
#define UART_REG_LSR 0x05
#define UART_REG_THR 0x00
#define UART_REG_RHR 0x00
#define UART_CLOCK_HZ 1843200UL
#define UART_BAUD 115200UL

#define UART_LCR_DLAB 0x80U
#define UART_LCR_8N1 0x03U
#define UART_FCR_ENABLE 0x01U
#define UART_FCR_CLEAR_RX 0x02U
#define UART_FCR_CLEAR_TX 0x04U
#define UART_LSR_TX_EMPTY 0x20U

/*----------------------- UART Config ------------------------*/

#if (configNUMBER_OF_CORES != 1)
static volatile uint32_t ulUartLock = 0U;

static inline void prvUartLockAcquire(void) {
  while (__atomic_exchange_n(&ulUartLock, 1U, __ATOMIC_ACQUIRE) != 0U) {
    __asm volatile("nop" ::: "memory");
  }
}

static inline void prvUartLockRelease(void) {
  __atomic_store_n(&ulUartLock, 0U, __ATOMIC_RELEASE);
}
#else
static inline void prvUartLockAcquire(void) {}
static inline void prvUartLockRelease(void) {}
#endif

/*-----------------------------------------------------------*/

void prvUartInit(void) {
  const uint16_t usDivisor = (uint16_t)(UART_CLOCK_HZ / (UART_BAUD * 16UL));

  *(volatile uint8_t*)(UART0_BASE + UART_REG_LCR) = UART_LCR_DLAB;
  *(volatile uint8_t*)(UART0_BASE + UART_REG_DLL) =
      (uint8_t)(usDivisor & 0xFFU);
  *(volatile uint8_t*)(UART0_BASE + UART_REG_DLM) = (uint8_t)(usDivisor >> 8);
  *(volatile uint8_t*)(UART0_BASE + UART_REG_LCR) = UART_LCR_8N1;
  *(volatile uint8_t*)(UART0_BASE + UART_REG_FCR) =
      UART_FCR_ENABLE | UART_FCR_CLEAR_RX | UART_FCR_CLEAR_TX;
  *(volatile uint8_t*)(UART0_BASE + UART_REG_MCR) = 0x00U;
}

/*-----------------------------------------------------------*/

void prvUartPutRaw(char c) {
  volatile uint8_t* const pLSR = (volatile uint8_t*)(UART0_BASE + UART_REG_LSR);
  volatile uint8_t* const pTHR = (volatile uint8_t*)(UART0_BASE + UART_REG_THR);
  uint32_t spins = 0;
  /* Bounded wait to avoid pathological stalls if UART status misbehaves. */
  while (((*pLSR) & UART_LSR_TX_EMPTY) == 0U) {
    if (++spins > 1000000U) {
      break;
    }
  }
  *pTHR = (uint8_t)c;
}

/*-----------------------------------------------------------*/

void prvUartWrite(const char* pcData, size_t xLength) {
  prvUartLockAcquire();

  for (size_t i = 0U; i < xLength; i++) {
    char c = pcData[i];
    if (c == '\n') {
      prvUartPutRaw('\r');
    }
    prvUartPutRaw(c);
  }

  prvUartLockRelease();
}

/*-----------------------------------------------------------*/

size_t prvWriteString(char* pcDest, const char* pcSrc) {
  size_t xLength = 0U;

  while (pcSrc[xLength] != '\0') {
    pcDest[xLength] = pcSrc[xLength];
    xLength++;
  }

  return xLength;
}

/*-----------------------------------------------------------*/

size_t prvWriteDecimal(char* pcDest, uint32_t ulValue) {
  char cTemp[10];
  size_t xCount = 0U;

  do {
    cTemp[xCount++] = (char)('0' + (ulValue % 10UL));
    ulValue /= 10UL;
  } while (ulValue != 0UL);

  for (size_t i = 0U; i < xCount; i++) {
    pcDest[i] = cTemp[xCount - 1U - i];
  }

  return xCount;
}

/*-----------------------------------------------------------*/

size_t prvWriteHex(char* pcDest, uint32_t ulValue) {
  static const char pcHex[] = "0123456789ABCDEF";

  for (size_t i = 0U; i < 8U; i++) {
    uint32_t shift = 28U - (uint32_t)(i * 4U);
    pcDest[i] = pcHex[(ulValue >> shift) & 0x0FU];
  }

  return 8U;
}

/*-----------------------------------------------------------*/

// void vDemoLogString(const char* pcMsg) {
//   char cBuffer[128];
//   size_t xIndex = 0U;

//   if (pcMsg == NULL) {
//     pcMsg = "(null)";
//   }

//   cBuffer[xIndex++] = '[';
//   xIndex += prvWriteString(&cBuffer[xIndex], "core ");
//   xIndex += prvWriteDecimal(&cBuffer[xIndex], (uint32_t)portGET_CORE_ID());
//   cBuffer[xIndex++] = ']';
//   cBuffer[xIndex++] = ' ';

//   /* 保持 vDemoLogString 原有语义：不强行补 '\n' */
//   size_t msgLen = strlen(pcMsg);
//   size_t spaceLeft = sizeof(cBuffer) - xIndex;
//   if (msgLen > spaceLeft) {
//     msgLen = spaceLeft;
//   }

//   for (size_t i = 0U; i < msgLen; i++) {
//     cBuffer[xIndex++] = pcMsg[i];
//   }

//   prvUartWrite(cBuffer, xIndex);
// }

/*-----------------------------------------------------------*/

// void vDemoLogDecimal(const char* pcLabel, uint32_t ulValue) {
//   char cBuffer[96];
//   size_t xIndex = 0U;

//   cBuffer[xIndex++] = '[';
//   xIndex += prvWriteString(&cBuffer[xIndex], "core ");
//   xIndex += prvWriteDecimal(&cBuffer[xIndex], (uint32_t)portGET_CORE_ID());
//   cBuffer[xIndex++] = ']';
//   cBuffer[xIndex++] = ' ';
//   xIndex += prvWriteString(&cBuffer[xIndex], pcLabel);
//   cBuffer[xIndex++] = ' ';
//   xIndex += prvWriteDecimal(&cBuffer[xIndex], ulValue);
//   cBuffer[xIndex++] = '\n';

//   prvUartWrite(cBuffer, xIndex);
// }

/*-----------------------------------------------------------*/

// void vDemoLogHexPair(const char* pcLabel, uint32_t ulFirst,
//                             uint32_t ulSecond) {
//   char cBuffer[128];
//   size_t xIndex = 0U;

//   cBuffer[xIndex++] = '[';
//   xIndex += prvWriteString(&cBuffer[xIndex], "core ");
//   xIndex += prvWriteDecimal(&cBuffer[xIndex], (uint32_t)portGET_CORE_ID());
//   cBuffer[xIndex++] = ']';
//   cBuffer[xIndex++] = ' ';
//   xIndex += prvWriteString(&cBuffer[xIndex], pcLabel);
//   cBuffer[xIndex++] = ' ';
//   xIndex += prvWriteDecimal(&cBuffer[xIndex], ulFirst);
//   xIndex += prvWriteString(&cBuffer[xIndex], " checksum=0x");
//   xIndex += prvWriteHex(&cBuffer[xIndex], ulSecond);
//   cBuffer[xIndex++] = '\n';

//   prvUartWrite(cBuffer, xIndex);
// }

/*-------------------重定义printf----------------------*/

static size_t prvAppendMem(char* pcDest, size_t xMax, size_t xIndex,
                           const char* pcSrc, size_t xLen) {
  if (xIndex >= xMax) {
    return xIndex;
  }
  if (xLen > (xMax - xIndex)) {
    xLen = xMax - xIndex;
  }
  if (xLen > 0U) {
    memcpy(&pcDest[xIndex], pcSrc, xLen);
    xIndex += xLen;
  }
  return xIndex;
}

static size_t prvAppendStringSafe(char* pcDest, size_t xMax, size_t xIndex,
                                  const char* pcStr) {
  if (pcStr == NULL) {
    pcStr = "(null)";
  }
  while ((*pcStr != '\0') && (xIndex < xMax)) {
    pcDest[xIndex++] = *pcStr++;
  }
  return xIndex;
}

static size_t prvAppendUnsignedDec(char* pcDest, size_t xMax, size_t xIndex,
                                   uint32_t ulValue) {
  char cTemp[10];
  size_t xCount = prvWriteDecimal(cTemp, ulValue);
  return prvAppendMem(pcDest, xMax, xIndex, cTemp, xCount);
}

static size_t prvAppendSignedDec(char* pcDest, size_t xMax, size_t xIndex,
                                 int32_t lValue) {
  uint32_t ulAbs;
  if (lValue < 0) {
    if (xIndex < xMax) {
      pcDest[xIndex++] = '-';
    }
    ulAbs = (uint32_t)(-lValue);
  } else {
    ulAbs = (uint32_t)lValue;
  }
  return prvAppendUnsignedDec(pcDest, xMax, xIndex, ulAbs);
}

static size_t prvAppendHex32(char* pcDest, size_t xMax, size_t xIndex,
                             uint32_t ulValue) {
  char cTemp[8];
  size_t xCount = prvWriteHex(cTemp, ulValue);
  return prvAppendMem(pcDest, xMax, xIndex, cTemp, xCount);
}

static size_t prvAppendPtr(char* pcDest, size_t xMax, size_t xIndex,
                           const void* pv) {
  uintptr_t val = (uintptr_t)pv;
  if (xIndex < xMax) {
    pcDest[xIndex++] = '0';
  }
  if (xIndex < xMax) {
    pcDest[xIndex++] = 'x';
  }
  return prvAppendHex32(pcDest, xMax, xIndex, (uint32_t)val);
}

/* 核心：把 format + va_list 格式化到 buffer 里 -----------------*/

static size_t prvFormatToBuffer(char* pcDest, size_t xMax, size_t xIndex,
                                const char* pcFmt, va_list xArgs) {
  char ch;

  while (((ch = *pcFmt++) != '\0') && (xIndex < xMax)) {
    if (ch != '%') {
      pcDest[xIndex++] = ch;
      continue;
    }

    ch = *pcFmt++;
    if (ch == '\0') {
      break;
    }

    if (ch == '%') {
      if (xIndex < xMax) {
        pcDest[xIndex++] = '%';
      }
      continue;
    }

    if (ch == 'l') {
      ch = *pcFmt++;
      if (ch == '\0') {
        break;
      }
    }

    switch (ch) {
      case 'c': {
        int c = va_arg(xArgs, int);
        if (xIndex < xMax) {
          pcDest[xIndex++] = (char)c;
        }
        break;
      }

      case 's': {
        const char* pcStr = va_arg(xArgs, const char*);
        xIndex = prvAppendStringSafe(pcDest, xMax, xIndex, pcStr);
        break;
      }

      case 'd':
      case 'i': {
        int32_t lVal = va_arg(xArgs, int32_t);
        xIndex = prvAppendSignedDec(pcDest, xMax, xIndex, lVal);
        break;
      }

      case 'u': {
        uint32_t ulVal = va_arg(xArgs, uint32_t);
        xIndex = prvAppendUnsignedDec(pcDest, xMax, xIndex, ulVal);
        break;
      }

      case 'x':
      case 'X': {
        uint32_t ulVal = va_arg(xArgs, uint32_t);
        xIndex = prvAppendHex32(pcDest, xMax, xIndex, ulVal);
        break;
      }

      case 'p': {
        void* pv = va_arg(xArgs, void*);
        xIndex = prvAppendPtr(pcDest, xMax, xIndex, pv);
        break;
      }

      default: {
        if (xIndex < xMax) {
          pcDest[xIndex++] = '%';
        }
        if (xIndex < xMax) {
          pcDest[xIndex++] = ch;
        }
        break;
      }
    }
  }

  if (xIndex < xMax) {
    pcDest[xIndex] = '\0';
  } else if (xMax > 0U) {
    pcDest[xMax - 1U] = '\0';
  }

  return xIndex;
}

/* 带/不带 core 前缀的通用输出函数 -------------------------------*/

static void prvVPrintfInternal(BaseType_t xWithCorePrefix, const char* pcFmt,
                               va_list xArgs) {
  char cBuffer[256];
  size_t xIndex = 0U;

  if (xWithCorePrefix != pdFALSE) {
    if (xIndex < sizeof(cBuffer)) {
      cBuffer[xIndex++] = '[';
    }
    xIndex = prvAppendStringSafe(cBuffer, sizeof(cBuffer), xIndex, "core ");
    xIndex = prvAppendUnsignedDec(cBuffer, sizeof(cBuffer), xIndex,
                                  (uint32_t)portGET_CORE_ID());
    if (xIndex < sizeof(cBuffer)) {
      cBuffer[xIndex++] = ']';
    }
    if (xIndex < sizeof(cBuffer)) {
      cBuffer[xIndex++] = ' ';
    }
  }

  xIndex = prvFormatToBuffer(cBuffer, sizeof(cBuffer), xIndex, pcFmt, xArgs);

  prvUartWrite(cBuffer, xIndex);
}

/* 对外接口 ---------------------------------------------------*/

/* 纯 printf 风格，不带 core 前缀 */
void vUartPrintf(const char* pcFmt, ...) {
  va_list xArgs;
  va_start(xArgs, pcFmt);
  prvVPrintfInternal(pdFALSE, pcFmt, xArgs);
  va_end(xArgs);
}

/* 带 [core X] 前缀并自动加换行的 printf */
void vLogPrintfLn(const char* pcFmt, ...) {
  va_list xArgs;
  va_start(xArgs, pcFmt);

  char cBuffer[256];
  size_t xIndex = 0U;

  /* 前缀：[core X] */
  if (xIndex < sizeof(cBuffer)) cBuffer[xIndex++] = '[';
  xIndex = prvAppendStringSafe(cBuffer, sizeof(cBuffer), xIndex, "core ");
  xIndex = prvAppendUnsignedDec(cBuffer, sizeof(cBuffer), xIndex,
                                (uint32_t)portGET_CORE_ID());
  if (xIndex < sizeof(cBuffer)) cBuffer[xIndex++] = ']';
  if (xIndex < sizeof(cBuffer)) cBuffer[xIndex++] = ' ';

  /* 格式化正文 */
  xIndex = prvFormatToBuffer(cBuffer, sizeof(cBuffer), xIndex, pcFmt, xArgs);

  // /* 自动加换行（如果还有空间）*/
  // if (xIndex < sizeof(cBuffer)) {
  //   cBuffer[xIndex++] = '\n';
  // }

  va_end(xArgs);

  /* 一次性写 UART（内部有锁） */
  prvUartWrite(cBuffer, xIndex);
}
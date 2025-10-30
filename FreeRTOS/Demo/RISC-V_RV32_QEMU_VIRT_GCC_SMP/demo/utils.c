#include <stddef.h>

void * memset( void * pvDest, int iValue, size_t xBytes )
{
    unsigned char * pucDest = ( unsigned char * ) pvDest;
    unsigned char ucValue = ( unsigned char ) iValue;

    while( xBytes-- )
    {
        *pucDest++ = ucValue;
    }

    return pvDest;
}

void * memcpy( void * pvDest, const void * pvSrc, size_t xBytes )
{
    unsigned char * pucDest = ( unsigned char * ) pvDest;
    const unsigned char * pucSrc = ( const unsigned char * ) pvSrc;

    while( xBytes-- )
    {
        *pucDest++ = *pucSrc++;
    }

    return pvDest;
}

void * memmove( void * pvDest, const void * pvSrc, size_t xBytes )
{
    unsigned char * pucDest = ( unsigned char * ) pvDest;
    const unsigned char * pucSrc = ( const unsigned char * ) pvSrc;

    if( pucDest == pucSrc )
    {
        return pvDest;
    }

    if( pucDest < pucSrc )
    {
        while( xBytes-- )
        {
            *pucDest++ = *pucSrc++;
        }
    }
    else
    {
        pucDest += xBytes;
        pucSrc += xBytes;

        while( xBytes-- )
        {
            *--pucDest = *--pucSrc;
        }
    }

    return pvDest;
}

int memcmp( const void * pvMem1, const void * pvMem2, size_t xBytes )
{
    const unsigned char * pucMem1 = ( const unsigned char * ) pvMem1;
    const unsigned char * pucMem2 = ( const unsigned char * ) pvMem2;

    while( xBytes-- )
    {
        int iDiff = ( int ) *pucMem1++ - ( int ) *pucMem2++;

        if( iDiff != 0 )
        {
            return iDiff;
        }
    }

    return 0;
}

size_t strlen( const char * pcString )
{
    const char * pcWalker = pcString;

    while( *pcWalker != '\0' )
    {
        pcWalker++;
    }

    return ( size_t ) ( pcWalker - pcString );
}

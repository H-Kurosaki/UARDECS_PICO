#ifndef _MYETH2_UTIL_H
#define _MYETH2_UTIL_H

// lwIPやその他のネットワークライブラリが定義済みか確認
#if defined(__LWIP_DEF_H__) || defined(_LWIP_DEF_H)
  // lwIPが定義済み: そのまま使用（何もしない）
  
#elif defined(htons) && defined(htonl)
  // 他のライブラリで定義済み: そのまま使用
  
#else
  // 未定義の場合のみ独自実装を提供
  #define htons(x) ( ((x)<<8 & 0xFF00) | \
                     ((x)>>8 & 0x00FF) )
  #define ntohs(x) htons(x)
  
  #define htonl(x) ( ((x)<<24 & 0xFF000000UL) | \
                     ((x)<< 8 & 0x00FF0000UL) | \
                     ((x)>> 8 & 0x0000FF00UL) | \
                     ((x)>>24 & 0x000000FFUL) )
  #define ntohl(x) htonl(x)
#endif

#endif // _MYETH2_UTIL_H
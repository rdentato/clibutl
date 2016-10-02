/*
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
**     
**                        ___   __
**                     __/  /_ /  )
**             ___  __(_   ___)  /
**            /  / /  )/  /  /  /
**           /  (_/  //  (__/  / 
**          (____,__/(_____(__/
**    https://github.com/rdentato/clibutl
**
*/

#include "utl.h"

#if 0

static uint8_t LEN[] = {1,1,1,1,2,2,3,0};
static uint8_t MSK[] = {0xFF,0xFF,0x1F,0x0F,0x07,0,0,0};
static int utf8_cp2(char * restrict txt, int32_t * restrict ch)
{
  int len = 0;
  int32_t val = 0;
  uint8_t *s = (uint8_t *)txt;
  unsigned first = *s;
   
  len = (first > 0) * (1 + ((first & 0xC0) == 0xC0) * LEN[(first >> 3) & 7]);
  val = first & MSK[len];
  
  switch (len) {  
    case 4: if ((*++s & 0xC0) != 0x80) goto invalid;
            val = (val << 6) | (*s & 0x3F);
            /* falls through! */
    case 3: if ((*++s & 0xC0) != 0x80) goto invalid;
            val = (val << 6) | (*s & 0x3F);
            /* falls through! */
    case 2: if ((*++s & 0xC0) != 0x80) goto invalid;
            val = (val << 6) | (*s & 0x3F);
            break;
    invalid: val = first; len = 1;
  }
 
  *ch = val;
  return len;
}
#endif

/*
1 	7 	U+0000   U+007F 	  0xxxxxxx
2 	11 	U+0080 	 U+07FF 	  110xxxxx 	10xxxxxx
3 	16 	U+0800 	 U+FFFF 	  1110xxxx 	10xxxxxx 	10xxxxxx
4 	21 	U+10000  U+10FFFF 	11110xxx 	10xxxxxx 	10xxxxxx 	10xxxxxx

11 00 0xxx 1 0x1F 
11 00 1xxx 1 0x1F 
11 01 0xxx 1 0x1F 
11 01 1xxx 1 0x1F 

11 10 0xxx 2 0x0F
11 10 1xxx 2 0x0F
11 11 0xxx 3 0x07
11 11 1xxx 0 

11 xx xxxx

*/



#define UTF8_ACCEPT 0
#define UTF8_REJECT 1

static const uint8_t utf8d[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
  8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
  0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
  0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
  0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
  1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
  1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
  1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
};

static int utf8_cp1(const char *txt, int32_t *ch)
{
  int len ;
  uint8_t * s = (uint8_t *)txt;
  uint8_t first = *s;
  uint32_t state = UTF8_ACCEPT;
  uint32_t type;
  
  if (*s > 0xC2) {
    type = utf8d[*s];
    *ch = (0xff >> type) & (*s);
    state = utf8d[256 + state*16 + type];
    s++;len=2;
    if (state > UTF8_REJECT) {
      type = utf8d[*s];
      *ch = (*s & 0x3fu) | (*ch << 6);
      state = utf8d[256 + state*16 + type];
      s++;len=2;
      if (state > UTF8_REJECT) {
        type = utf8d[*s];
        *ch = (*s & 0x3fu) | (*ch << 6);
        state = utf8d[256 + state*16 + type];
        s++;len=3;
        if (state > UTF8_REJECT) {
          type = utf8d[*s];
          *ch = (*s & 0x3fu) | (*ch << 6);
          state = utf8d[256 + state*16 + type];
          s++;len=4;
        }
      }
    }
    if (state != UTF8_ACCEPT) {*ch = first; len = 1;}
  }
  else { *ch = *s; len = (*ch > 0); }  
  return len;
}

static int utf8_cp2(const char * txt, int32_t * ch)
{
  int len;
  uint8_t *s = (uint8_t *)txt;
  uint8_t first = *s;
    
  fsm {
    fsmSTART {
      if (*s <= 0xC1) { *ch = *s; len = (*s > 0); fsmGOTO(end);    }
      if (*s <= 0xDF) { *ch = *s & 0x1F; len = 2; fsmGOTO(len2);   }
      if (*s == 0xE0) { *ch = *s & 0x0F; len = 3; fsmGOTO(len3_0); }
      if (*s <= 0xEC) { *ch = *s & 0x0F; len = 3; fsmGOTO(len3_1); }
      if (*s == 0xED) { *ch = *s & 0x0F; len = 3; fsmGOTO(len3_2); }
      if (*s <= 0xEF) { *ch = *s & 0x0F; len = 3; fsmGOTO(len3_1); }
      if (*s == 0xF0) { *ch = *s & 0x07; len = 4; fsmGOTO(len4_0); }
      if (*s <= 0xF3) { *ch = *s & 0x07; len = 4; fsmGOTO(len4_1); }
      if (*s == 0xF4) { *ch = *s & 0x07; len = 4; fsmGOTO(len4_2); }
      fsmGOTO(invalid);
    } 
    
    fsmSTATE(len4_0) {
      s++; if ( *s < 0x90 || 0xbf < *s) fsmGOTO(invalid);
      *ch = (*ch << 6) | (*s & 0x3F);
      fsmGOTO(len3_1);
    }
    
    fsmSTATE(len4_1) {
      s++; if ( *s < 0x80 || 0xbf < *s) fsmGOTO(invalid);
      *ch = (*ch << 6) | (*s & 0x3F);
      fsmGOTO(len3_1);
    }
    
    fsmSTATE(len4_2) {
      s++; if ( *s < 0x80 || 0x8f < *s) fsmGOTO(invalid);
      *ch = (*ch << 6) | (*s & 0x3F);
      fsmGOTO(len3_1);
    }
    
    fsmSTATE(len3_0) {
      s++; if ( *s < 0xA0 || 0xbf < *s) fsmGOTO(invalid);
      *ch = (*ch << 6) | (*s & 0x3F);
      fsmGOTO(len2);
    }
    
    fsmSTATE(len3_1) {
      s++; if ( *s < 0x80 || 0xbf < *s) fsmGOTO(invalid);
      *ch = (*ch << 6) | (*s & 0x3F);
      fsmGOTO(len2);
    }

    fsmSTATE(len3_2) {
      s++; if ( *s < 0x80 || 0x9f < *s) fsmGOTO(invalid);
      *ch = (*ch << 6) | (*s & 0x3F);
      fsmGOTO(len2);
    }
    
    fsmSTATE(len2) {
      s++; if ( *s < 0x80 || 0xbf < *s) fsmGOTO(invalid);
      *ch = (*ch << 6) | (*s & 0x3F);
      fsmGOTO(end);
    }
    
    fsmSTATE(invalid) {*ch = first; len = 1;}
    
    fsmSTATE(end)   { }
  }
  return len;
}

const char *jp = "概要\
1992年4月1日に阪急電鉄がそれまで乗車券購入や運賃精算用として販売していたラガールカードを用いたストアードフェアシステム「ラガールスルー」の運用を開始し、1994年には能勢電鉄が自社のパストラルカードとラガールカードを共通化する形で参加。そしてこれをベースとして当時自動改札機の更新を予定していた阪神電気鉄道、大阪市交通局、北大阪急行電鉄の3社局にも拡張対応させ、1996年3月20日から「スルッとKANSAI」の統一名称を用いての運用を開始した。\
当初、スルッとKANSAIに関わる業務は加盟各社局が分担して行っていたが、加盟社局の増加により、業務の効率化を図るため、専任事務局として株式会社スルッとカンサイが2000年7月18日に設立された。\
スルッとKANSAIでは、ストアードフェアシステムの提携のみにとどまらず、各種チケットや、グッズ制作販売会社とともに加盟各社のグッズなどの企画を行い、各社主要駅などでバンダイや明治製菓（現：明治）などと共同で企画した玩具やチューインガムなどを販売している。共同企画の玩具やCDやバンダイから発売されているBトレインショーティーの限定版は、基本的に一度販売されれば再発売しない方式を貫いている[2]。また、乗車券用紙などの資材の一括調達も行っている（ただし、入札によることが要求される公営交通機関はこの一括調達から外れる）。\
2004年からは非接触型ICカード「PiTaPa」が導入された。これによりスルッとKANSAI協議会には近畿圏の交通事業者だけではなく、岡山地区や静岡県の交通事業者も加盟して「PiTaPa」を導入するようになった。また、「PiTaPa」は電子マネーとしても利用できることから、自治体なども参加している（参加事業者・導入時期は「PiTaPa」の項目を参照）。\
\
三重県で最大手のバス会社である三重交通では、協議会に加盟した後、2016年4月1日より、自社専用のIC乗車カード「emica」の導入に合わせて、PiTaPaのみの導入が行われた。\
\
また、自治体などが自主運行するコミュニティバスに関しては、加盟社局であっても利用できない路線もある。\
\
スルッとKANSAI対応カードの発売額面は以下の通りである。なお、社局によっては取り扱いのない額面がある。また、いずれのカードも有効期限は設定されていない。\
滋賀県";

const char *in = "ঠার এহান টটারতারা মানু ঔতায় ঠার এহানরে ’ইমার ঠার’ বুলিয়া মাততারা। ঠার এহান আগে হুদ্দা ’মণিপুরী’ \
বুলিয়া নাঙ পালসিল, পিসে পিসেদে মণিপুরর অন্যান্য জাতি উতারাঙতো তঙাল করতেগা ’বিষ্ণুপ্রিয়া’ ৱাহি এগো ’মণিপুরী’র আগে বহিল বারো\
 ’বিষ্ণুপ্রিয়া মনিপুরী’ বুলিয়া পরিচিত অইল। ভাষাবিদ গিরকলকেইর মতে ’বিষ্ণুপ্রিয়া’ ৱাহি এগো আহেসেতা ’বিষ্ণুপুর’\
 এত্ত যেহান আগে মণিপুরর রাজধানীহান আসিল; ’বিষ্ণুপুর’র লগে ’ইয়া’ প্রত্যয় যোগ করিয়া বিষ্ণুপুরীয়া ক্রমে উহাত্ত বিষ্ণুপ্রিয়া। তবে বিষ্ণুপ্রিয়া \
 মণিপুরী বুজন গিরীগিথানীয়ে এপাকাউ তানুরে বভ্রুবাহনর বংশধর বারো আর্যবংশীয় ত্রিয় বুলিয়া পরিচয় দিতারা। বভ্রুবাহন অইলতায় অর্জ্জুন বারো\
 চিত্রাঙ্গদার পুতক। মহাভারতর আদিপর্ব্ব বারো অশ্বমেধপর্ব্বে তৃতীয় পান্ডব অর্জ্জুন বারো মণিপুরর রাজকন্যা চিত্রাঙ্গদার পরিচয়স্থান বুলিয়া উলেখ আসে।\
 তানুর মতে বভ্রুবাহন নিয়াম বিষ্ণুভক্তগ আসিল অহানে বিষ্ণুর ভক্ত অনায় বিষ্ণুপ্রিয়া বুলতারা। বিষ্ণুপ্রিয়া মনিপুরী ঠারর উৎসগল ভাষাবিদ ইতিহাসবিদ \
 উতারাঙ দুহান মত প্রচলিত আছে। ঐতিহাসিক শ্রীমহেন্দ্র কুমার সিংহ বারো অন্যান্য লেখক যেমন জগৎমোহন সিংহ, বীরেন্দ্র কুমার সিংহ, মঙ্গলবাবু সিংহ\
 প্রমুখ গিরকগাছিয়ে ঠার এহানরে মহাভারতে উলেখিত বভ্রুবাহনর লগে যেতা হস্তিনাপুরেত্ব মণিপুরে আহেসিলা তানুর সৃষ্ট প্রাচীন ভাষাহান বুলিয়া বারো এহার উ\
 ৎসগ শৌরসেনী প্রাকৃত বুলিয়া মাতে গেসিগা। ঐতিহাসিক রাজমোহন নাথ গিরকে এহানর সমর্থন জানাসে তার The Background of Assamese culture\
 লেরিকে। ড. কালীপ্রসাদ সিংহ গিরকে ঠার এহার বাক্যরীতি, ধ্বনিতত্ব ,রূপতত্বর শব্দার্থতত্ব বারো ইতিহাসমুলক ভাষাতত্বলো মুল ফাৎকরা গবেষনা করেসে। গিরকের \
 ";
     
int main(int argc, char *argv[])
{
  const char *s;
  int32_t ch2,ch1;
  int l2, l1;
  
  logopen("l_utf8.log","w");
  
  s = "";
  l2 = utf8_cp2(s,&ch2);
  l1 = utf8_cp1(s,&ch1);
  logcheck(l2 == l1);
  logcheck( ch2 ==  ch1);
  logprintf("%s %d %x %d %x",s,l1,ch1,l2,ch2);
  
  s = "a";
  l2 = utf8_cp2(s,&ch2);
  l1 = utf8_cp1(s,&ch1);
  logcheck(l2 == l1);
  logcheck( ch2 ==  ch1);
  logprintf("%s %d %x %d %x",s,l1,ch1,l2,ch2);
    
  s = "à";
  l2 = utf8_cp2(s,&ch2);
  l1 = utf8_cp1(s,&ch1);
  logcheck(l2 == l1);
  logcheck( ch2 ==  ch1);
  logprintf("%s %d %x %d %x",s,l1,ch1,l2,ch2);
  
  s = "も";
  l2 = utf8_cp2(s,&ch2);
  l1 = utf8_cp1(s,&ch1);
  logcheck(l2 == l1);
  logcheck( ch2 ==  ch1);
  logprintf("%s %d %x %d %x",s,l1,ch1,l2,ch2);
  
  s = "𫀔";
  l2 = utf8_cp2(s,&ch2);
  l1 = utf8_cp1(s,&ch1);
  logcheck(l2 == l1);
  logcheck( ch2 ==  ch1);
  logprintf("%s %d %x %d %x",s,l1,ch1,l2,ch2);

  s = "\xFF";
  l2 = utf8_cp2(s,&ch2);
  l1 = utf8_cp1(s,&ch1);
  logcheck(l2 == l1);
  logcheck( ch2 ==  ch1);
  logprintf("%s %d %x %d %x",s,l1,ch1,l2,ch2);

  s = "\xF0";
  l2 = utf8_cp2(s,&ch2);
  l1 = utf8_cp1(s,&ch1);
  logcheck(l2 == l1);
  logcheck( ch2 ==  ch1);
  logprintf("%s %d %x %d %x",s,l1,ch1,l2,ch2);

  s = "\xF0\x80\x80\xBF";
  l2 = utf8_cp2(s,&ch2);
  l1 = utf8_cp1(s,&ch1);
  logcheck(l2 == l1);
  logcheck( ch2 ==  ch1);
  logprintf("%s %d %x %d %x",s,l1,ch1,l2,ch2);

  s = "\xFF\x80";
  l2 = utf8_cp2(s,&ch2);
  l1 = utf8_cp1(s,&ch1);
  logcheck(l2 == l1);
  logcheck( ch2 ==  ch1);
  logprintf("%s %d %x %d %x",s,l1,ch1,l2,ch2);

  if (1){
    int max = 2000000;
    int k = max;
    int l; int32_t c; const char *t; 
    logclock {
      while (k--) {
        t = "aàも𫀔";
        l = utf8_cp2(t,&c); t+=l;
        l = utf8_cp2(t,&c); t+=l;
        l = utf8_cp2(t,&c); t+=l;
        l = utf8_cp2(t,&c); t+=l;
      }
    }
    k = max;
    l = 0;
    logclock {
      while (k--) {
        t = "aàも𫀔";
        l = utf8_cp2(t,&c); t+=l;
        l = utf8_cp2(t,&c); t+=l;
        l = utf8_cp2(t,&c); t+=l;
        l = utf8_cp2(t,&c); t+=l;
      }
    }
  }
  
  if (1) {
    int max = 20000000;
    int k = max;
    logclock {
     int l; int32_t c; const char *t=jp;
      while (k--) {
        do {
          l = utf8_cp1(t,&c); t+=l;
        }
        while (l>0);
      }
    }
    k = max;
    logclock {
     int l; int32_t c; const char *t=jp;
      while (k--) {
        do {
          l = utf8_cp2(t,&c); t+=l;
        }
        while (l>0);
      }
    }
  }

  if (1) {
    int max = 20000000;
    int k = max;
    logclock {
     int l; int32_t c; const char *t=in;
      while (k--) {
        do {
          l = utf8_cp1(t,&c); t+=l;
        }
        while (l>0);
      }
    }
    k = max;
    logclock {
     int l; int32_t c; const char *t=in;
      while (k--) {
        do {
          l = utf8_cp2(t,&c); t+=l;
        }
        while (l>0);
      }
    }
  }
    
#if 0 
  if (0) {
  const char *t; int l; int32_t c;
  
  t = "aàも𫀔";
  while(1) {
    l = utf8_cp2(t, &c);
    printf("'%s' len:%d cp:0x%05x\n", t, l, c);
    if (*t == 0) break;
    t += l;
  } 
  
  }
#endif 

  logclose();
  exit(0);
}



/*
 * tools.c: Tools for handling configuration files and strings
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "tools.h"

//
// HTML conversion code taken from RSS Reader plugin for VDR
// http://www.saunalahti.fi/~rahrenbe/vdr/rssreader/
// by Rolf Ahrenberg
//

// --- Static -----------------------------------------------------------

#define ELEMENTS(x) (sizeof(x) / sizeof(x[0]))

struct conv_table {
  const char *from;
  const char *to;
};

static struct conv_table pre_conv_table[] =
{
  // 'to' field must be smaller than 'from'
  {"<br />",   "\n"}
};

// Conversion page: http://www.ltg.ed.ac.uk/~richard/utf-8.cgi

static struct conv_table post_conv_table[] =
{
  // 'to' field must be smaller than 'from'
  {"&quot;",   "\x22"},
  {"&#34;",    "\x22"},
  {"&amp;",    "\x26"},
  {"&#38;",    "\x26"},
  {"&#038;",   "\x26"},
  {"&#039;",   "\x27"},
  {"&#40;",    "\x28"},
  {"&#41;",    "\x29"},
  {"&#58;",    "\x3a"},
  {"&lt;",     "\x3c"},
  {"&#60;",    "\x3c"},
  {"&gt;",     "\x3e"},
  {"&#62;",    "\x3e"},
  {"&#91;",    "\x5b"},
  {"&#93;",    "\x5d"},
  {"&nbsp;",   "\xc2\xa0"},
  {"&#160;",   "\xc2\xa0"},
  {"&deg;",    "\xc2\xb0"},
  {"&#176;",   "\xc2\xb0"},
  {"&acute;",  "\xc2\xb4"},
  {"&#180;",   "\xc2\xb4"},
  {"&Auml;",   "\xc3\x84"},
  {"&#196;",   "\xc3\x84"},
  {"&Aring;",  "\xc3\x85"},
  {"&#197;",   "\xc3\x85"},
  {"&Ouml;",   "\xc3\x96"},
  {"&#214;",   "\xc3\x96"},
  {"&Uuml;",   "\xc3\x9c"},
  {"&#220;",   "\xc3\x9c"},
  {"&szlig;",  "\xc3\x9f"},
  {"&#223;",   "\xc3\x9f"},
  {"&acirc;",  "\xc3\xa2"},
  {"&#226;",   "\xc3\xa2"},
  {"&auml;",   "\xc3\xa4"},
  {"&#228;",   "\xc3\xa4"},
  {"&aring;",  "\xc3\xa5"},
  {"&#229;",   "\xc3\xa5"},
  {"&ccedil;", "\xc3\xa7"},
  {"&#231;",   "\xc3\xa7"},
  {"&eacute;", "\xc3\xa9"},
  {"&#233;",   "\xc3\xa9"},
  {"&ecirc;",  "\xc3\xaa"},
  {"&#234;",   "\xc3\xaa"},
  {"&ouml;",   "\xc3\xb6"},
  {"&#246;",   "\xc3\xb6"},
  {"&uuml;",   "\xc3\xbc"},
  {"&#252;",   "\xc3\xbc"},
  {"&ndash;",  "\xe2\x80\x93"},
  {"&#8211;",  "\xe2\x80\x93"},
  {"&mdash;",  "\xe2\x80\x94"},
  {"&#8212;",  "\xe2\x80\x94"},
  {"&lsquo;",  "\xe2\x80\x98"},
  {"&#8216;",  "\xe2\x80\x98"},
  {"&rsquo;",  "\xe2\x80\x99"},
  {"&#8217;",  "\xe2\x80\x99"},
  {"&sbquo;",  "\xe2\x80\x9a"},
  {"&#8218;",  "\xe2\x80\x9a"},
  {"&ldquo;",  "\xe2\x80\x9c"},
  {"&#8220;",  "\xe2\x80\x9c"},
  {"&rdquo;",  "\xe2\x80\x9d"},
  {"&#8221;",  "\xe2\x80\x9d"},
  {"&bdquo;",  "\xe2\x80\x9e"},
  {"&#8222;",  "\xe2\x80\x9e"},
  {"&prime;",  "\xe2\x80\xb3"},
  {"&#8243;",  "\xe2\x80\xb3"},
  {"&euro;",   "\xe2\x82\xac"},
  {"&#8364;",  "\xe2\x82\xac"},
  {"\n\n",     "\n"}, // let's also strip multiple linefeeds
};

static char *htmlcharconv(char *str, struct conv_table *conv, unsigned int elem)
{
  if (str && conv) {
     for (unsigned int i = 0; i < elem; ++i) {
        char *ptr = strstr(str, conv[i].from);
        while (ptr) {
           long of = ptr - str;
           size_t l  = strlen(str);
           size_t l1 = strlen(conv[i].from);
           size_t l2 = strlen(conv[i].to);
           if (l2 > l1) {
              error("htmlcharconv(): cannot reallocate string");
              return str;
              }
           if (l2 != l1)
              memmove(str + of + l2, str + of + l1, l - of - l1 + 1);
           strncpy(str + of, conv[i].to, l2);
           ptr = strstr(str, conv[i].from);
           }
        }
     return str;
     }
  return NULL;
}

// --- General functions ------------------------------------------------

char *striphtml(char *str)
{
  if (str) {
     char *c, t = 0, *r;
     str = htmlcharconv(str, pre_conv_table, ELEMENTS(pre_conv_table));
     c = str;
     r = str;
     while (*str != '\0') {
       if (*str == '<')
          t++;
       else if (*str == '>')
          t--;
       else if (t < 1)
          *(c++) = *str;
       str++;
       }
     *c = '\0';
     return htmlcharconv(r, post_conv_table, ELEMENTS(post_conv_table));
     }
  return NULL;
}

cListItem::cListItem()
{
  enabled = false;
  string = NULL;
  numchannels = 0;
  channels_num = NULL;
  channels_str = NULL;
}

cListItem::~cListItem(void)
{
  Free();
}

void cListItem::Free(void)
{
  if (channels_str) {
     int i = 0;
     while (i < numchannels) {
           free(channels_str[i]);
           ++i;
           }
     }
  FREE(channels_num);
  FREE(channels_str);
  FREE(string);
  numchannels = 0;
  enabled = false;
}

const char *cListItem::GetChannelID(int index)
{
  if (channels_str && index >= 0 && index < numchannels)
     return channels_str[index];
  else
     return NULL;
}

int cListItem::GetChannelNum(int index)
{
  if (channels_num && index >= 0 && index < numchannels)
     return channels_num[index];
  else
     return 0;
}

bool cListItem::IsActive(tChannelID ChannelID)
{
  bool active = true;
  if (numchannels > 0) {
     bool found = false;
     int i = 0;
     while (i < numchannels) {
           if ((Channels.GetByChannelID(ChannelID)->Number() == GetChannelNum(i)) || 
               (GetChannelID(i) && strcmp(*(ChannelID.ToString()), GetChannelID(i)) == 0)) {
              found = true;
              break;
              }
           ++i;
           }
     if (!found)
        active = false;
     }
  return active;
}

int cListItem::LoadChannelsFromString(const char *string)
{
  numchannels = 0;
  char *tmpstring = strdup(string);
  char *c = tmpstring;
  while (c) {
        ++numchannels;
        c = strchr(c+1, ',');
        }
  if (numchannels > 0) {
     char *c = tmpstring;
     // Use channel numbers
     if (atoi(string))
        channels_num = (int *)malloc(sizeof(int)*numchannels);
     else// use channel IDs
        channels_str = (char **)malloc(sizeof(char *)*numchannels);
     int i = 0;
     char *pc = strtok(c, ",");
     while (i < numchannels) {
           // Use channel numbers
           if (atoi(string))
              (channels_num)[i] = atoi(pc);
           else// use channel IDs
              (channels_str)[i] = strdup(pc);
           pc = strtok(NULL, ",");
           ++i;
           }
     }
  free(tmpstring);
  return numchannels;
}

void cListItem::ToggleEnabled(void)
{
  enabled = !enabled;
}

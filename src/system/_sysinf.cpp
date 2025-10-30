/*******************************************************************************
+
+  LEDA 7.2.2  
+
+
+  _sysinf.c
+
+
+  Copyright (c) 1995-2025
+  by Algorithmic Solutions Software GmbH
+  All rights reserved.
+ 
*******************************************************************************/


#include <LEDA/system/file.h>
#include <LEDA/core/string.h>

#include <string.h>
#include <stdlib.h>

#if defined(__win32__) || defined(__CYGWIN32__)|| defined(__win64__)
/*
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN
*/

#if defined(MAXINT)
#undef MAXINT
#endif

#include <windows.h>


LEDA_BEGIN_NAMESPACE

//void get_host_info(char* name, unsigned long sz, list<unsigned long> &idlist)
int get_host_info(char* name, int sz, unsigned long* idlist, int max_ids)
{ 
  unsigned long maxcomp = 0;
  unsigned long filesysflag = 0;
  unsigned long hostid = 0;

  GetVolumeInformation("c:\\",NULL,0,(DWORD*)&hostid,(DWORD*)&maxcomp,
                                                     (DWORD*)&filesysflag,0,0);
  GetComputerName(name,(DWORD*)&sz);
  if (hostid == 0) hostid = 0x1739ce18;
  idlist[0] = hostid;
  return 1;
}

LEDA_END_NAMESPACE

#else

#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#if  defined(hpux) || defined (__DECCXX) || defined(__xlC__)

#if defined(__DECCXX) || defined(hpux)
extern "C" unsigned long gethostid();
#endif

LEDA_BEGIN_NAMESPACE

int get_host_info(char* name, int sz, unsigned long* idlist, int max_ids)
{ 
  gethostname(name,sz);
  unsigned long hostid = (unsigned long)gethostid();
  if (hostid == 0) hostid = 0x1739ce18;
  idlist[0] = hostid;
  return 1;
}

LEDA_END_NAMESPACE

#elif defined(__svr4__)

#include <sys/systeminfo.h>

LEDA_BEGIN_NAMESPACE

int get_host_info(char* name, int sz, long, unsigned long* idlist, int max_ids)
{ 
  sysinfo(SI_HOSTNAME,name,sz);
  char buf[32];
  sysinfo(SI_HW_SERIAL,buf,32);
  unsigned long hostid = strtoul(buf,0,0);
  if (hostid == 0) hostid = 0x1739ce18;
  idlist[0] = hostid;
  return 1;
}

LEDA_END_NAMESPACE

#elif defined(linux) && !defined(__ANDROID__)

#include<sys/socket.h>

#include<fcntl.h>
#include<iostream>
#include<stdio.h>
#include<errno.h>
#include<string.h>
#include<net/if.h>
#include<sys/ioctl.h>
#include<stdlib.h>
#include<LEDA/core/list.h>

LEDA_BEGIN_NAMESPACE

// this function retrieves the MAC addresses of all network
// device interfaces that are no LOOPBACK devices
// the retrieved MAC addresses are converted to hostids by using
// last 4 bytes of the 6 byte MAC address

//void get_macs(leda::list<unsigned long>& L)
int get_macs(unsigned long* ID_List, int sz)
{
  int count = 0;

  struct ifreq ifr;
  struct ifreq *IFR;
  struct ifconf ifc;
  char *buf;
  int s, i;

  unsigned char addr[6];

// create socket	
  s = socket(AF_INET, SOCK_DGRAM, 0);
  if (s==-1) return 0;
    
// determine list of interfaces
  int lastlen=0;
  int len = 1024;
  for(; ;)
  {
    buf = new char[len];	
    ifc.ifc_len = len;
    ifc.ifc_buf = buf;

    if(ioctl(s, SIOCGIFCONF, &ifc)<0)
    {
       if(errno !=EINVAL || lastlen !=0) 
       { delete[] buf;
         return 0;
        }
    }
    else
    { if(ifc.ifc_len == lastlen) break;
      lastlen = ifc.ifc_len;
    }
    len = 2*len;
    delete[] buf;
  }


  //loop through list of interfaces
  IFR = ifc.ifc_req;
   
  for (i = ifc.ifc_len / sizeof(struct ifreq); --i >= 0; IFR++) 
  {
    strcpy(ifr.ifr_name, IFR->ifr_name);
    //test if interface is loopback device
    if (ioctl(s, SIOCGIFFLAGS, &ifr) == 0) 
    {
      if (! (ifr.ifr_flags & IFF_LOOPBACK)) 
      {
      // this is a network card, determine MAC address
        if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0) 
        {
          bcopy( ifr.ifr_hwaddr.sa_data, addr, 6);
          // MAC address is 6 byte
          unsigned long id = 0;
          for(int i=2;i<5;i++)
          {
            id |=(unsigned int) addr[i];
            id =id<< 8;
          }
          id |= (unsigned int) addr[5];
          //L.append(id);
          ID_List[count++] = id;
        }
      }
    }
  }

  delete[] buf;
  close(s);

  return count;
}




int get_host_info(char* name, int sz, unsigned long* idlist, int max_ids)
{ 
  gethostname(name,sz);

  //retrieve MAC-like hostids
  //get_macs(idlist);

  int count = get_macs(idlist,max_ids);

  // append "old" hostid for backward compatibility (remove this for LEDA-5.1)
  unsigned long hostid = gethostid();
  if (hostid != 0) {
     idlist[count++] = hostid;
  }

  // if we could not retrieve any hostid, use a dummy
  if(count == 0) idlist[count++] = 0x1739ce18;

  return count;
}

LEDA_END_NAMESPACE

#else

LEDA_BEGIN_NAMESPACE

//void get_host_info(char* name, unsigned long, list<unsigned long> &idlist)
int get_host_info(char* name, int, unsigned long* idlist, int max_ids)
{ 
  strcpy(name,"????");
  idlist[0] = 0x1739ce18;
  return 1;
}
 
LEDA_END_NAMESPACE

#endif

#endif


/*
 * packetsend - A utility to construct data packets from mixed sources
 * which can be e.g. strings, files, hex-string or checksums of data.
 *
 * The program was initially named 'tcpsend', but this name was increasingly
 * wrong as time progressed and output to serial ports, udp and files were
 * added.
 *
 * The program is licensed under the GNU GPL version 3 or later
 *
 * For the full text of the license, see the (possibly) included
 * LICENSE.md or online at: https://www.gnu.org/licenses/gpl-3.0.en.html
 */

#define	_DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <termios.h>

#define	FMT_NONE		0
#define	FMT_RAW		1
#define	FMT_HEX		2
#define	FMT_BHEX		4
#define	FMT_CLEAN	5
#define	FMT_AHEX		6
#define	FMT_BBIN		7
#define	FMT_BDEC		8
#define	FMT_BBIT		9

#define	ACK_NONE		0
#define	ACK_CHECK	1
#define	ACK_SINGLE	2
#define	ACK_MULTI	3

#define	BUFSIZE	256
#define	DEFTIMEOUT	1000	/* Milliseconds */
#define	DEF_SERVPORT	5702

// NOLINTBEGIN(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling,clang-analyzer-security.insecureAPI.strcpy)
#pragma GCC diagnostic ignored "-Wpointer-sign"
#pragma GCC diagnostic ignored "-Wparentheses"

enum KeyVal {
	KEY_CSTYPE,
	KEY_ACK,
	KEY_ACHECK,
	KEY_QUIET,
	KEY_TEXT,
	KEY_FILE,
	KEY_HELP,
	KEY_HEX,
	KEY_IP,
	KEY_TIMEOUT,
	KEY_PORT,
	KEY_VERSION,
	KEY_CS,
	KEY_DEBUG,
	KEY_PROTO,
	KEY_DEV,
	KEY_BAUD,
	KEY_FLOW,
	KEY_OUTFMT,
	KEY_MARK,
	KEY_MATCH,
	KEY_DELAY,
	KEY_CRCSEED,
	KEY_CRCXOR,
	KEY_CRCKEY,
	KEY_CRCMODE,
	KEY_INVALID
};

static const struct {
	char *s;
	enum KeyVal Class;
	int Len;
} keylist[]={
	{"getreply=",KEY_ACK,9},
	{"outfmt=",KEY_OUTFMT,7},
	{"checkreply",KEY_ACHECK,11},
	{"quiet",KEY_QUIET,6},
	{"text=",KEY_TEXT,5},
	{"timeout=",KEY_TIMEOUT,8},
	{"file=",KEY_FILE,5},
	{"hex=",KEY_HEX,4},
	{"ip=",KEY_IP,3},
	{"port=",KEY_PORT,5},
	{"proto=",KEY_PROTO,6},
	{"cs=",KEY_CS,3},
	{"cstype=",KEY_CSTYPE,7},
	{"replymatch=",KEY_MATCH,11},
	{"dev=",KEY_DEV,4},
	{"device=",KEY_DEV,7},
	{"baud=",KEY_BAUD,5},
	{"flow=",KEY_FLOW,5},
	{"mark",KEY_MARK,5},
	{"delay=",KEY_DELAY,6},
	{"crcseed=",KEY_CRCSEED,8},
	{"crcxor=",KEY_CRCXOR,7},
	{"crckey=",KEY_CRCKEY,7},
	{"crcmode=",KEY_CRCMODE,8},
	{"-h",KEY_HELP,3},
	{"-debug",KEY_DEBUG,7},
	{"-v",KEY_VERSION,3},
	{"--help",KEY_HELP,6},
	{"--version",KEY_VERSION,9},
	{0,0,0}
};

static char *strepl(char * restrict oldstr,char * restrict newstr) {
	char *ret=0;
	if(oldstr) {
		free(oldstr);
	}
	if(newstr) {
		ret=malloc(strlen(newstr)+1);
		strcpy(ret,newstr);
	}
	return ret;
}

static void sleepus(long int ms) {
	struct timespec ts={
		.tv_sec=ms/1000,
		.tv_nsec=1000*(ms%1000)
	};
	nanosleep(&ts,0);
}

#define	DEBUGP(s,args...)	if(Debug){printf(s , ## args);}

int BTranslate(int b) {
#define BTRANS(x)	case x:return(B ## x)
switch(b) {
BTRANS(50);
BTRANS(75);
BTRANS(110);
BTRANS(134);
BTRANS(150);
BTRANS(200);
BTRANS(300);
BTRANS(600);
BTRANS(1200);
BTRANS(2400);
BTRANS(4800);
BTRANS(9600);
BTRANS(19200);
BTRANS(38400);
BTRANS(57600);
BTRANS(115200);
BTRANS(230400);
}
#undef BTRANS
fprintf(stderr,"Invalid baudrate!\n");
return(B2400);
}

static void PrintData(unsigned char *data,int len,int fmt);
static int ReadData(int zock,unsigned char *buf,int blen,int timeout,int mode,unsigned char *mbuf,int mpos);
static uint8_t CRC8(uint8_t *data,int dlen,uint8_t div,uint8_t initial,uint8_t finxor,int rin,int rout);
static uint16_t CRC16(uint8_t *data,int dlen,uint16_t div,uint16_t initial,uint16_t finxor,int rin,int rout);
static uint32_t CRC32(uint8_t *data,int dlen,uint32_t div,uint32_t initial,uint32_t finxor,int rin,int rout);


static int Debug=0;

static void sigh(int signo)
{
return;
}

static const struct {
	const int bit;
	const char * const name;
} PinNames[]={
	{TIOCM_LE,"DSR/LE"},
	{TIOCM_DTR,"DTR"},
	{TIOCM_RTS,"RTS"},
	{TIOCM_ST,"ST"},
	{TIOCM_SR,"SR"},
	{TIOCM_CTS,"CTS"},
	{TIOCM_CAR,"CAR"},
	{TIOCM_RNG,"RNG"},
	{TIOCM_DSR,"DSR"},
	{0,0}
};

static void PrintPins(char *m,int bits) {
	if(Debug) {
		int i=-1;
		DEBUGP("%s",m);
		while(PinNames[++i].name) {
			if(bits&PinNames[i].bit) {
				DEBUGP("%s ",PinNames[i].name);
			}
		}
		DEBUGP("\n");
	}
}

static void ManualPins(int fd,int enable,int disable) {
	int bits,abits;
	if(!ioctl(fd,TIOCMGET,&bits)) {
		PrintPins("Original pins: ",bits);
		bits=(bits|enable)&~disable;
		PrintPins("Setting pins to: ",bits);
		ioctl(fd,TIOCMSET,&bits);
		ioctl(fd,TIOCMGET,&abits);
		PrintPins("Actual pins: ",abits);
	} else {
		DEBUGP("Failed to get serial pins.\n");
	}
}

int main(int cnt,char *arg[])
{
int Ret=0,ArgKey;
int NoRestore=0;
int i,n,t;
int l,bp;
int Proto=IPPROTO_TCP;
char HexBuf[3]={0};
FILE *fp=0;
struct stat Statbuf;
char *ServIP;
int ServPort=DEF_SERVPORT,RemoteAck=ACK_NONE,Baud=2400,Flow=0,CFlow=0,Quiet=0,ReadToggle=0;
int SendDelay=0,
ManualPins_Read_Set=0,
ManualPins_Read_Clear=0,
ManualPins_Write_Set=0,
ManualPins_Write_Clear=0;
int OutFmt=FMT_NONE;
char *Device;
char *q,*ap=0;
unsigned char *DataBuf;
long int zock=0;
struct sockaddr_in target;
struct timeval tv={1,0};
int res,csStart=-1,csEnd=-1;
int32_t	Hold1,Timeout=DEFTIMEOUT;
int CSStartMark=-1,CSStopMark=-1;
unsigned char MatchBuf[64];
int mpos=0;
enum {
	CS_Sum,
	CS_ZSum,
	CS_ASCIISum4,
	CS_CRC8,
	CS_CRC16,
	CS_CRC32
} CSType=CS_Sum;
uint8_t c8=0x1c;
uint16_t c16=0x1021;
uint32_t c32=0x04c11db7;
uint32_t crc_initial=0,crc_xor=0;
int crc_rin=0,crc_rout=0;

DataBuf=malloc(32768);
bp=0;
ServIP=strepl(0,"127.0.0.1");
Device=strepl(0,"/dev/ttyS0");

if(!ServIP || !DataBuf || !Device) {	/* Initial check. */
	fprintf(stderr,"Error: No free store!\n");
	Ret=20;
	goto cleanup;
}

for(i=1;!Ret && i<cnt;i++) {
	n=-1;
	ArgKey=KEY_INVALID;
	while(keylist[++n].s!=0) {
		if(!strncmp(arg[i],keylist[n].s,keylist[n].Len)) {
			ArgKey=keylist[n].Class;
			l=keylist[n].Len;
			ap=&arg[i][l];
			break;
		}
	}
	switch(ArgKey) {

	case	KEY_CSTYPE:
		if(!strcmp(ap,"zsum")) {
			CSType=CS_ZSum;
		} else if(!strcmp(ap,"sum")) {
			CSType=CS_Sum;
		} else if(!strcmp(ap,"asciisum4")) {
			CSType=CS_ASCIISum4;
		} else if(!strcmp(ap,"crc8") || !strcmp(ap,"CRC8")) {
			CSType=CS_CRC8;
		} else if(!strcmp(ap,"crc16") || !strcmp(ap,"CRC16")) {
			CSType=CS_CRC16;
		} else if(!strcmp(ap,"crc32") || !strcmp(ap,"CRC32")) {
			CSType=CS_CRC32;
		} else {
			fprintf(stderr,"Invalid checksum type.\n");
			CSType=CS_ZSum;
		}
	break;

	case	KEY_DELAY:
		SendDelay=(int)(1000*atof(ap));
		if(ap<0) {
			SendDelay=0;
		}
	break;

	case	KEY_CRCSEED:
		crc_initial=strtol(ap,0,16);
	break;

	case	KEY_CRCXOR:
		crc_xor=strtol(ap,0,16);
	break;

	case	KEY_CRCKEY:
		c32=strtol(ap,0,16);
		c16=c32&0xffff;
		c8=c16&0xff;
	break;

	case	KEY_CRCMODE:
		if(!strcmp(ap,"none")) {
			crc_rin=0;
			crc_rout=0;
		} else if(!strcmp(ap,"data")) {
			crc_rin=1;
			crc_rout=0;
		} else if(!strcmp(ap,"result")) {
			crc_rin=0;
			crc_rout=1;
		} else if(!strcmp(ap,"both")) {
			crc_rin=1;
			crc_rout=1;
		} else {
			fprintf(stderr,"Invalid CRC mode.\n");
		}
	break;

	case	KEY_CS:
		if(ap[0]=='-') {
			if(!strncmp(&ap[1],"stop",4)) {
				csEnd=CSStopMark-1;
			} else {
				csEnd=atoi(&ap[1]);
			}
		} else {
			if(!strncmp(ap,"start",5)) {
				csStart=CSStartMark;
				q=0;
			} else {
				csStart=strtol(ap,&q,0);
			}
			if(q && q[1]!=0) {
				if(!strncmp(&q[1],"stop",4)) {
					csEnd=CSStopMark-1;
				} else {
					csEnd=atoi(&q[1]);
				}
			} else {
				csEnd=32767;
			}
		}
		if(csStart>csEnd) {
			csStart=csEnd=-1;
		}
		if(csEnd>bp) {
			csEnd=bp;
		}
		
		if(csStart>=0 && csEnd>=0) {
			Hold1=0;
			switch(CSType) {
			case	CS_Sum:
				for(t=csStart;t<=csEnd;t++) {
					Hold1+=DataBuf[t];
				}
				DataBuf[bp++]=(Hold1&0xff);
			break;
			case	CS_ZSum:
				for(t=csStart;t<=csEnd;t++) {
					Hold1+=DataBuf[t];
				}
				Hold1=-Hold1;
				DataBuf[bp++]=(Hold1&0xff);
			break;
			case CS_ASCIISum4:
				for(t=csStart;t<=csEnd;t++) {
					Hold1+=DataBuf[t];
				}
				snprintf(&DataBuf[bp],5,"%04X",Hold1);
				bp+=4;
			break;
			case	CS_CRC8:
				c8=CRC8(&DataBuf[csStart],csEnd-csStart+1,c8,crc_initial&0xff,crc_xor&0xff,crc_rin,crc_rout);
				DataBuf[bp++]=c8;
			break;
			case	CS_CRC16:
				c16=CRC16(&DataBuf[csStart],csEnd-csStart+1,c16,crc_initial&0xffff,crc_xor&0xffff,crc_rin,crc_rout);
				DataBuf[bp++]=c16>>8;
				DataBuf[bp++]=c16&0xff;
			break;
			case	CS_CRC32:
				c32=CRC32(&DataBuf[csStart],csEnd-csStart+1,c32,crc_initial,crc_xor,crc_rin,crc_rout);
				DataBuf[bp++]=c32>>24;
				DataBuf[bp++]=(c32>>16)&0xff;
				DataBuf[bp++]=(c32>>8)&0xff;
				DataBuf[bp++]=c32&0xff;
			break;
			}
		}
	break;

	case	KEY_DEBUG:
		Debug=1;
	break;

	case	KEY_ACK:
		if(!strcmp(ap,"first") || !strcmp(ap,"single")) {
			RemoteAck=ACK_SINGLE;
		} else if(!strcmp(ap,"timeout") || !strcmp(ap,"multi")) {
			RemoteAck=ACK_MULTI;
		} else if(!strcmp(ap,"check")) {
			RemoteAck=ACK_CHECK;
		} else {
			fprintf(stderr,"Bad reply style specified to getreply.\n");
		}
	break;

	case	KEY_OUTFMT:
		if(!strcmp(ap,"raw")) {
			OutFmt=FMT_RAW;
		} else if(!strcmp(ap,"none")) {
			OutFmt=FMT_NONE;
		} else if(!strcmp(ap,"hex")) {
			OutFmt=FMT_HEX;
		} else if(!strcmp(ap,"ahex")) {
			OutFmt=FMT_AHEX;
		} else if(!strcmp(ap,"bhex")) {
			OutFmt=FMT_BHEX;
		} else if(!strcmp(ap,"bbin")) {
			OutFmt=FMT_BBIN;
		} else if(!strcmp(ap,"bdec")) {
			OutFmt=FMT_BDEC;
		} else if(!strcmp(ap,"bbit")) {
			OutFmt=FMT_BBIT;
		} else if(!strcmp(ap,"clean")) {
			OutFmt=FMT_CLEAN;
		} else {
			OutFmt=FMT_NONE;
			fprintf(stderr,"Invalid output format.\n");
		}
	break;

	case	KEY_ACHECK:
		RemoteAck=ACK_CHECK;
	break;
	
	case	KEY_TIMEOUT:
		Timeout=1000*atof(ap);
		if(Timeout<0) {
			Timeout=1000;
		}
		DEBUGP("Timeout=%ldms\n",(long int)Timeout);
	break;
	
	case	KEY_IP:
		ServIP=strepl(ServIP,ap);
	break;

	case	KEY_PORT:
		ServPort=atoi(ap);
	break;

	case KEY_QUIET:
		Quiet=1;
	break;

	case	KEY_TEXT:
		strcpy(&DataBuf[bp],ap);
		bp+=strlen(ap);
	break;

	case	KEY_FILE:
		if(!stat(ap,&Statbuf)) {
			if(fp=fopen(ap,"rb")) {
				fread(&DataBuf[bp],1,Statbuf.st_size,fp);
				bp+=Statbuf.st_size;
				fclose(fp);
			} else {
				fprintf(stderr,"Could not open %s.\n",ap);
				Ret=1;
			}
		} else {
			fprintf(stderr,"Could not stat %s.\n",ap);
			Ret=1;
		}
	break;

	case	KEY_DEV:
		Device=strepl(Device,ap);
	break;

	case	KEY_BAUD:
		Baud=atoi(ap);
	break;

	case	KEY_HEX:
		for(t=0;t<(strlen(ap)>>1);t++) {
			HexBuf[0]=ap[2*t];
			HexBuf[1]=ap[2*t+1];
			DataBuf[bp++]=strtol(HexBuf,0,16);
		}
	break;

	case KEY_MATCH:
		for(t=0;t<(strlen(ap)>>1);t++) {
			HexBuf[0]=ap[2*t];
			HexBuf[1]=ap[2*t+1];
			MatchBuf[mpos++]=strtol(HexBuf,0,16);
		}
	break;

	case	KEY_VERSION:
		printf("TCPSend v0.0.0.0.0.0.0.0.0.0.7.2.1.3.8.0\n"\
		"Compiled: %s, %s.\n",__DATE__,__TIME__);
		Ret=1;
	break;

	case	KEY_HELP:
		printf(
			"packsend [options] [keywords]\n"
			"\tcheckreply      Returnvalue of 1 if no reply was received.\n"
			"\tquiet           Fewer error messages.\n"
			"\tgetreply=<mode> Waits for a reply. mode is in {first,timeout}\n"
			"\toutfmt=<x>      With getreply. Output format is in {raw,hex,ahex,bhex,bdec,bbin,clean,bbit,none}.\n"
			"\ttext=<text>     Appends the specified text to the data to send.\n"
			"\ttimeout=<s>     Sets the timeout to <s> seconds (default 1).\n"
			"\treplymatch=<x>  Hex-sequence of bytes to wait for (with timeout).\n"
			"\tfile=<name>     Appends the contents of the file <name> to the data to send.\n"
			"\thex=<hex>       Appends the hex string <hex> as bytes to the data to send.\n"
			"\tip=<ip>         The ip to send the data to.\n"
			"\tport=<port>     The port to send the data to.\n"
			"\tproto=<proto>   Either \"tcp\", \"udp\", \"serial\" (8N1) or \"stdout\".\n"
			"\tmark            Set a mark to use for start/stop for checksums. First mark is start, second is stop.\n"
			"\tcs=<from>-<to>  Appends a checksum of the bytes <from> through <to>. If <from>=start or <to>=stop, use marks.\n"
			"\tcstype=<type>   Checksum type, in {sum,zsum,asciisum4,crc8,crc16,crc32}.\n"
			"\tcrcseed=<hex>   Seed value for the CRC, default 0.\n"
			"\tcrcxor=<hex>    Final xor value for the CRC, default 0.\n"
			"\tcrckey=<hex>    CRC polynomial, default is 0x1C/0x1021/0x04c11db7 for 8/16/32-bit.\n"
			"\tcrcmode=<x>     CRC reflection mode in {none,data,result,both}. Default is none.\n"
			"\tdelay=<time>    The delay in seconds before starting to send. (default 0)\n"
			"\tdev=<device>    Sets the serial device to <device> (Default is /dev/ttyS0).\n"
			"\tbaud=<speed>    Sets the baudrate for serial communication.\n"
			"\tflow=<x>[,<x>]  Set serial parameters. x is in {[no]rtscts,[no]local,[no]ignbrk,readtoggle,[no]{rts|cts|dtr|dsr|rng}on{read|write}}.\n"
			"\t-h              This text.\n"
			"\t--help          This text.\n"
			"\t-debug          Show some more info when running.\n"
			"\t-v              Show version.\n"
			"\t--version       Show version.\n"
			"\n");
		Ret=1;
	break;

	case	KEY_FLOW:
		if(strstr(ap,"nortscts")) {
			CFlow|=CRTSCTS;
		} else if(strstr(ap,"rtscts")) {
			Flow|=CRTSCTS;
		}
		if(strstr(ap,"nolocal")) {
			CFlow|=CLOCAL;
		} else if(strstr(ap,"local")) {
			Flow|=CLOCAL;
		}
		if(strstr(ap,"noignbrk")) {
			CFlow|=IGNBRK;
		} else if(strstr(ap,"ignbrk")) {
			Flow|=IGNBRK;
		}
		if(strstr(ap,"readtoggle")) {
			ReadToggle=1;
		}
		if(strstr(ap,"noleonwrite")) {
			ManualPins_Write_Clear|=TIOCM_LE;
		} else if(strstr(ap,"leonwrite")) {
			ManualPins_Write_Set|=TIOCM_LE;
		}
		if(strstr(ap,"noleonread")) {
			ManualPins_Read_Clear|=TIOCM_LE;
		} else if(strstr(ap,"leonread")) {
			ManualPins_Read_Set|=TIOCM_LE;
		}
		if(strstr(ap,"nortsonwrite")) {
			ManualPins_Write_Clear|=TIOCM_RTS;
		} else if(strstr(ap,"rtsonwrite")) {
			ManualPins_Write_Set|=TIOCM_RTS;
		}
		if(strstr(ap,"noctsonwrite")) {
			ManualPins_Write_Clear|=TIOCM_CTS;
		} else if(strstr(ap,"ctsonwrite")) {
			ManualPins_Write_Set|=TIOCM_CTS;
		}
		if(strstr(ap,"nodtronwrite")) {
			ManualPins_Write_Clear|=TIOCM_DTR;
		} else if(strstr(ap,"dtronwrite")) {
			ManualPins_Write_Set|=TIOCM_DTR;
		}
		if(strstr(ap,"nodsronwrite")) {
			ManualPins_Write_Clear|=TIOCM_DSR;
		} else if(strstr(ap,"dsronwrite")) {
			ManualPins_Write_Set|=TIOCM_DSR;
		}
		if(strstr(ap,"norngonwrite")) {
			ManualPins_Write_Clear|=TIOCM_RNG;
		} else if(strstr(ap,"rngonwrite")) {
			ManualPins_Write_Set|=TIOCM_RNG;
		}
		if(strstr(ap,"nortsonread")) {
			ManualPins_Read_Clear|=TIOCM_RTS;
		} else if(strstr(ap,"rtsonread")) {
			ManualPins_Read_Set|=TIOCM_RTS;
		}
		if(strstr(ap,"noctsonread")) {
			ManualPins_Read_Clear|=TIOCM_CTS;
		} else if(strstr(ap,"ctsonread")) {
			ManualPins_Read_Set|=TIOCM_CTS;
		}
		if(strstr(ap,"nodtronread")) {
			ManualPins_Read_Clear|=TIOCM_DTR;
		} else if(strstr(ap,"dtronread")) {
			ManualPins_Read_Set|=TIOCM_DTR;
		}
		if(strstr(ap,"nodsronread")) {
			ManualPins_Read_Clear|=TIOCM_DSR;
		} else if(strstr(ap,"dsronread")) {
			ManualPins_Read_Set|=TIOCM_DSR;
		}
		if(strstr(ap,"norngonread")) {
			ManualPins_Read_Clear|=TIOCM_RNG;
		} else if(strstr(ap,"rngonread")) {
			ManualPins_Read_Set|=TIOCM_RNG;
		}
		PrintPins("Read Set   : ",ManualPins_Read_Set);
		PrintPins("Read Clear : ",ManualPins_Read_Clear);
		PrintPins("Write Set  : ",ManualPins_Write_Set);
		PrintPins("Write Clear: ",ManualPins_Write_Clear);
	break;

	case	KEY_MARK:
		if(CSStartMark==-1) {
			CSStartMark=bp;
		} else {
			CSStopMark=bp;
		}
	break;

	case	KEY_PROTO:
		if(!strcmp(ap,"tcp")) {
			Proto=IPPROTO_TCP;
		} else if(!strcmp(ap,"udp")) {
			Proto=IPPROTO_UDP;
		} else if(!strcmp(ap,"serial")) {
			Proto=-1;
		} else if(!strcmp(ap,"stdout")) {
			Proto=-2;
		} else {
			fprintf(stderr,"Bad protocol, using TCP.\n");
			Proto=IPPROTO_TCP;
		}
	break;

	default:
		fprintf(stderr,"Unrecognised option \"%s\".\n",arg[i]);
		Ret=5;
	}
}

if(Ret) {
	goto cleanup;
}

if(Proto==IPPROTO_TCP) {
	zock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

	if(zock!=-1) {
		DEBUGP("Created socket, will attempt to connect.\n");
		memset(&target,0,sizeof(target));
		target.sin_family=AF_INET;
		target.sin_port=htons(ServPort);
		target.sin_addr.s_addr=inet_addr(ServIP);
		res=connect(zock,(struct sockaddr *)&target,sizeof(target));
	} else {
		if(!Quiet) {
			fprintf(stderr,"Unable to create socket, errno=%d.\n",errno);
		}
		Ret=5;
		goto cleanup;
	}

	if(res!=-1) {
		DEBUGP("Connection established.\n");
		if(SendDelay>0) {
			DEBUGP("Waiting before sending data.\n");
			sleepus(1000*SendDelay);
		}
		send(zock,DataBuf,bp,0);
		DEBUGP("Data sent.\n");
	
		if(RemoteAck) {
			if((l=ReadData(zock,DataBuf,32768,Timeout,RemoteAck,MatchBuf,mpos))>=0) {
				PrintData(DataBuf,l,OutFmt);
			} else {
				DEBUGP("Answer timed out.\n");
				Ret=1;
			}
		}
		
	} else {
		if(!Quiet) {
			perror("connect() failed ");
			fprintf(stderr,"Unable to connect to server, errno=%d.\n",errno);
		}
		Ret=10;
	}
	shutdown(zock,SHUT_RDWR);
} else if(Proto==IPPROTO_UDP) {
	zock=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(zock!=-1) {
		DEBUGP("Created socket.\n");
		target.sin_family=AF_INET;
		target.sin_port=htons(ServPort);
		target.sin_addr.s_addr=inet_addr(ServIP);
		if(SendDelay>0) {
			DEBUGP("Waiting before sending data.\n");
			sleepus(1000*SendDelay);
		}
		if(sendto(zock,DataBuf,bp,0,(struct sockaddr *)&target,sizeof(target))==-1) {
			Ret=10;
			if(!Quiet) {
				perror("sendto() failed ");
				fprintf(stderr,"Unable to send data.\n");
			}
		} else if(RemoteAck) {
			fd_set	rs;
			FD_ZERO(&rs);
			FD_SET(zock,&rs);
			DEBUGP("Waiting for answer.\n");
			close(zock);
			zock=socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
			res=bind(zock,(struct sockaddr *)&target,sizeof(target));
			if(res==-1) {
				if(!Quiet) {
					perror("bind() failed ");
					fprintf(stderr,"Unable to wait for reply.\n");
				}
				Ret=10;
			} else {
				if((l=ReadData(zock,DataBuf,32768,Timeout,RemoteAck,MatchBuf,mpos))>0) {
					PrintData(DataBuf,l,OutFmt);
				} else {
					DEBUGP("Answer timed out.\n");
					Ret=1;
				}
			}
		}
	} else {
		Ret=10;
		if(!Quiet) {
			perror("socket() failed ");
			fprintf(stderr,"Unable to create socket!\n");
		}
	}
} else if(Proto==-1) {	/* Serial */
	DEBUGP("Opening device.\n");
	zock=open(Device,O_NOCTTY|O_RDWR|O_NDELAY);
	if(zock==-1) {
		if(!Quiet) {
			fprintf(stderr,"Unable to open %s!\n",Device);
		}
		Ret=10;
	} else {
		struct termios tioset,oldtios;
		fd_set ws;
		i=fcntl(zock,F_GETFL,0)&~O_NDELAY;	/* Do not use NDELAY now. */
		fcntl(zock,F_SETFL,i);
		DEBUGP("Attempting to set serial attributes.\n");
		tcgetattr(zock,&oldtios);
		tcgetattr(zock,&tioset);
		tioset.c_iflag&=~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
		tioset.c_oflag&=~OPOST;
		tioset.c_lflag&=~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
		tioset.c_cflag&=~(CSIZE|PARENB|CSTOPB);
		tioset.c_cflag&=~CFlow;	/* Clear unwanted bits from flow control. */
		if(ReadToggle) {
			DEBUGP("Disabling read capability for write.\n");
			tioset.c_cflag|=(Flow|CS8|HUPCL);
			tioset.c_cflag&=~CREAD;
		} else {
			tioset.c_cflag|=(Flow|CS8|CREAD|HUPCL);
		}
		cfsetospeed(&tioset,BTranslate(Baud));
		cfsetispeed(&tioset,BTranslate(Baud));
		tcflush(zock,TCIOFLUSH);
		tcsetattr(zock,TCSANOW,&tioset);
		if(ManualPins_Write_Set || ManualPins_Write_Clear) {
			DEBUGP("Manual Pins.\n");
			ManualPins(zock,ManualPins_Write_Set,ManualPins_Write_Clear);
		}
		if(SendDelay>0) {
			DEBUGP("Waiting before sending data.\n");
			sleepus(1000*SendDelay);
		}
		FD_ZERO(&ws);
		FD_SET(zock,&ws);
		tv.tv_sec=Timeout/1000;
		tv.tv_usec=1000*(Timeout-1000*tv.tv_sec);
		DEBUGP("Testing write-ready state.\n");
		i=select(zock+1,0,&ws,0,&tv);
		if(i<0) {
			if(!Quiet) {
				fprintf(stderr,"Error waiting for port to be writeable.\n");
			}
			tcsetattr(zock,TCSANOW,&oldtios);
			close(zock);
			exit(5);
		}
		DEBUGP("Writing data.\n");

		i=write(zock,DataBuf,bp);
		if(i<bp) {
			Ret=10;
			if(!Quiet) {
				fprintf(stderr,"Write failed.\n");
			}
		} else {
			DEBUGP("Draining output.\n");
			signal(SIGALRM,sigh);
			alarm(3);
			i=tcdrain(zock);
			alarm(0);
			if(i) { 
				Ret=10;
				NoRestore=1;
				tcflush(zock,TCIOFLUSH);
				if(!Quiet) {
					fprintf(stderr,"Failed to drain output, errno=%d (will not reset state now).\n",errno);
				}
			} else {
				if(RemoteAck) {
					if(ManualPins_Read_Set || ManualPins_Read_Clear) {
						DEBUGP("Manual Pins.\n");
						ManualPins(zock,ManualPins_Read_Set,ManualPins_Read_Clear);
					}
					if(ReadToggle) {
						tioset.c_cflag|=CREAD;
						tcsetattr(zock,TCSANOW,&tioset);
					}
					if((l=ReadData(zock,DataBuf,32768,Timeout,RemoteAck,MatchBuf,mpos))>0) {
						PrintData(DataBuf,l,OutFmt);
					} else {
						DEBUGP("Answer timed out. Errno=%d\n",errno);
						Ret=1;
					}
				} /* Remoteack */
			}
		}
		if(!NoRestore) {
			DEBUGP("Restoring serial state.\n");
			tcsetattr(zock,TCSAFLUSH,&oldtios);
		}
	}
} else if(Proto==-2) {	/* stdout */
		i=write(STDOUT_FILENO,DataBuf,bp);
		if(i<bp) {
			Ret=10;
			if(!Quiet) {
				fprintf(stderr,"Write failed.\n");
			}
		} else {
			if(RemoteAck) {
				if((l=ReadData(STDIN_FILENO,DataBuf,32768,Timeout,RemoteAck,MatchBuf,mpos))>0) {
					PrintData(DataBuf,l,OutFmt);
				} else {
					DEBUGP("Answer timed out. Errno=%d\n",errno);
					Ret=1;
				}
			} /* Remoteack */
		}
} else {
	fprintf(stderr,"Bad protocol!\n");
	Ret=5;
}

DEBUGP("Closing connection.\n");
if(zock) {
	if(NoRestore) {	/* Possibility of hanging here? */
		alarm(2);
		close(zock);
		alarm(0);
	} else {
		close(zock);
	}
}

cleanup:

if(DataBuf) {
	free(DataBuf);
}
if(ServIP) {
	free(ServIP);
}
if(Device) {
	free(Device);
}

DEBUGP("Bye.\n");

return(Ret);
}

static char *BinStr(int n) {
	static char buf[9];
	int i;
	for(i=0;i<8;i++) {
		buf[7-i]=(n&1)?'1':'0';
		n>>=1;
	}
	buf[i]=0;
	return(buf);
}

static void PrintData(unsigned char *data,int len,int fmt)
{
int i,n,t;
unsigned char z;

switch(fmt) {
case	FMT_RAW:
	fwrite(data,1,len,stdout);
break;
case	FMT_HEX:
	for(i=0;i<len;i++) {
		printf("%02x",data[i]);
	}
	printf("\n");
break;
case	FMT_BHEX:
	for(i=0;i<len;i++) {
		printf("%02x ",data[i]);
	}
	printf("\n");
break;
case	FMT_AHEX:
	for(i=0;i<len;i++) {
		printf("%02x ",data[i]);
		if((i&15)==15) {
			for(n=0;n<16;n++) {
				z=data[i-15+n];
				printf("%c",(isgraph(z))?z:'.');
			}
			printf("\n");
		}
	}
	n=i-(i&15);
	for(i=(len-n);i<16;i++) {
		printf("   ");
	}
	for(i=n;i<len;i++) {
		z=data[i];
		printf("%c",(isgraph(z))?z:'.');
	}
	printf("\n");
break;
case	FMT_CLEAN:
	for(i=0;i<len;i++) {
		if(data[i]!='\r') {
			putchar(data[i]);
		}
	}
break;
case	FMT_BDEC:
	for(i=0;i<len;i++) {
		printf("%03d ",data[i]);
	}
	printf("\n");
break;
case	FMT_BBIN:
	for(i=0;i<len;i++) {
		printf("%s ",BinStr(data[i]));
	}
	printf("\n");
break;
case	FMT_BBIT:
	for(i=0;i<len;i++) {
		t=0x80;
		for(n=0;n<8;n++) {
			printf("%c ",(data[i]&t)?'1':'0');
		}
	}
	printf("\n");
break;
default:
	fprintf(stderr,"Internal error. Bad output format (%d).\n",fmt);
case	FMT_NONE:
	;
}

}

static int ReadData(int zock,unsigned char *buf,int blen,int timeout,int mode,unsigned char *ematch,int matchlen)
{
fd_set	rs;
int l=-1;
struct timeval tv;
struct itimerval itim,zero={{0,0},{0,0}};

if(mode==ACK_NONE) {
	return(0);
}

DEBUGP("Waiting for answer (%d.%d second timeout).\n",(timeout/1000),(timeout%1000));
if(matchlen>0) {
	DEBUGP("Also finishing on match of lenfth %d.\n",matchlen);
}
FD_ZERO(&rs);
FD_SET(zock,&rs);
l=0;
itim.it_interval.tv_sec=0;
itim.it_interval.tv_usec=0;
itim.it_value.tv_sec=timeout/1000;
itim.it_value.tv_sec=1000*(timeout-1000*tv.tv_sec);
tv.tv_sec=timeout/1000;
tv.tv_usec=1000*(timeout-1000*tv.tv_sec);

if(select(zock+1,&rs,0,0,&tv)>0) {
	setitimer(ITIMER_REAL,&itim,0);
	l=read(zock,buf,(matchlen>0)?1:blen);
	setitimer(ITIMER_REAL,&zero,0);
	if(matchlen>0 && l>=matchlen && !memcmp(ematch,&buf[l-matchlen],matchlen)) {
		return(l);
	}
	if(l>0 && mode==ACK_MULTI) {
		tv.tv_sec=timeout/1000;
		tv.tv_usec=1000*(timeout-1000*tv.tv_sec);
		DEBUGP("Initial answer acquired. Waiting for more data.\n");
		FD_ZERO(&rs);
		FD_SET(zock,&rs);
		while(select(zock+1,&rs,0,0,&tv)>0) {
			if(matchlen>0 && l>=matchlen && !memcmp(ematch,&buf[l-matchlen],matchlen)) {
				break;
			}
			setitimer(ITIMER_REAL,&itim,0);
			l+=read(zock,&buf[l],(matchlen>0)?1:32768);
			setitimer(ITIMER_REAL,&zero,0);
			tv.tv_sec=timeout/1000;
			tv.tv_usec=1000*(timeout-1000*tv.tv_sec);
			/*tv.tv_sec=timeout;
			tv.tv_usec=0;*/
			FD_ZERO(&rs);
			FD_SET(zock,&rs);
		}
	}
}

return(l);
}


/* Data reflection {{{ */
static uint32_t reflect32(uint32_t v) {
	uint32_t r=0;
	int i;
	for(i=0;i<32;i++) {
		if(v&1) {
			r|=1<<(31-i);
		}
		v>>=1;
	}
	return(r);
}

static uint16_t reflect16(uint16_t v) {
	uint16_t r=0;
	int i;
	for(i=0;i<16;i++) {
		if(v&1) {
			r|=1<<(15-i);
		}
		v>>=1;
	}
	return(r);
}

static uint8_t reflect8(uint8_t v) {
	uint8_t r=0;
	int i;
	for(i=0;i<8;i++) {
		if(v&1) {
			r|=1<<(7-i);
		}
		v>>=1;
	}
	return(r);
}
/*}}}*/

/* CRC8, generic {{{ */
static uint8_t CRC8(uint8_t *data,int dlen,uint8_t div,uint8_t initial,uint8_t finxor,int rin,int rout)
{
int i,n;
uint8_t rem=initial;

for(i=0;i<dlen;i++) {
	if(rin) {
		rem^=reflect8(data[i]);
	} else {
		rem^=data[i];
	}
	for(n=0;n<8;n++) {
		if(rem&0x80) {
			rem=(rem<<1)^div;
		} else {
			rem<<=1;
		}
	}
}

if(rout) {
	rem=reflect8(rem);
}

return(rem^finxor);
}
/*}}}*/

/* CRC16, generic {{{ */
static uint16_t CRC16(uint8_t *data,int dlen,uint16_t div,uint16_t initial,uint16_t finxor,int rin,int rout)
{
int i,n;
uint16_t rem=initial;

for(i=0;i<dlen;i++) {
	if(rin) {
		rem^=reflect8(data[i])<<8;
	} else {
		rem^=data[i]<<8;
	}
	for(n=0;n<8;n++) {
		if(rem&0x8000) {
			rem=(rem<<1)^div;
		} else {
			rem<<=1;
		}
	}
}

if(rout) {
	rem=reflect16(rem);
}

return(rem^finxor);
}
/*}}}*/

/* CRC32, generic {{{ */
static uint32_t CRC32(uint8_t *data,int dlen,uint32_t div,uint32_t initial,uint32_t finxor,int rin,int rout)
{
int i,n;
uint32_t rem=initial;

for(i=0;i<dlen;i++) {
	if(rin) {
		rem^=reflect8(data[i])<<24;
	} else {
		rem^=data[i]<<24;
	}
	for(n=0;n<8;n++) {
		if(rem&0x80000000) {
			rem=(rem<<1)^div;
		} else {
			rem<<=1;
		}
	}
}

if(rout) {
	rem=reflect32(rem);
}

return(rem^finxor);
}
/*}}}*/

// NOLINTEND(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling,clang-analyzer-security.insecureAPI.strcpy)



/*
 * SerialHeader.h
 *
 *  Created on: Aug 26, 2010
 *      Author: beat
 */

#ifndef SERIALHEADER_H_
#define SERIALHEADER_H_

#define SERHEADER_Compress 1<<0
#define SERHEADER_Version 1
#define SERIAL_MAGIC 0xfeedbabe

class SerialHeader
{
public:
  unsigned int m_magic;
  int flags;
  int version;
  int comp_version;
  long long ser_tim;
  long long run_number;
  int buffersize;
  SerialHeader() : m_magic(SERIAL_MAGIC),flags(0),version(SERHEADER_Version),comp_version(0),ser_tim(0),run_number(0),buffersize(0){}
  SerialHeader & operator = (const SerialHeader &t){memcpy(this,&t, sizeof(SerialHeader));return *this;}
};

#endif /* SERIALHEADER_H_ */

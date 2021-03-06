/*
 *      vdr-plugin-xvdr - XBMC server plugin for VDR
 *
 *      Copyright (C) 2010 Alwin Esch (Team XBMC)
 *      Copyright (C) 2010, 2011 Alexander Pipelka
 *
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#ifndef XVDR_RECEIVER_H
#define XVDR_RECEIVER_H

#include <linux/dvb/frontend.h>
#include <linux/videodev2.h>
#include <vdr/channels.h>
#include <vdr/device.h>
#include <vdr/receiver.h>
#include <vdr/thread.h>
#include <vdr/ringbuffer.h>

#include "demuxer/demuxer.h"
#include <list>

class cxSocket;
class cChannel;
class cLiveReceiver;
class cTSDemuxer;
class cResponsePacket;
class cLivePatFilter;
class cLiveQueue;

class cLiveStreamer : public cThread
                    , public cRingBufferLinear
{
private:
  friend class cTSDemuxer;
  friend class cLivePatFilter;
  friend class cChannelCache;

  void Detach(void);
  void Attach(void);
  cTSDemuxer *FindStreamDemuxer(int Pid);

  void reorderStreams(int lang, eStreamType type);

  void sendStreamPacket(sStreamPacket *pkt);
  void sendStreamChange();
  void sendSignalInfo();
  void sendStreamInfo();
  void sendStatus(int status);

  const cChannel   *m_Channel;                      /*!> Channel to stream */
  cDevice          *m_Device;                       /*!> The receiving device the channel depents to */
  cLiveReceiver    *m_Receiver;                     /*!> Our stream transceiver */
  cLivePatFilter   *m_PatFilter;                    /*!> Filter processor to get changed pid's */
  int               m_Priority;                     /*!> The priority over other streamers */
  std::list<cTSDemuxer*> m_Demuxers;
  cxSocket         *m_Socket;                       /*!> The socket class to communicate with client */
  int               m_Frontend;                     /*!> File descriptor to access used receiving device  */
  dvb_frontend_info m_FrontendInfo;                 /*!> DVB Information about the receiving device (DVB only) */
  v4l2_capability   m_vcap;                         /*!> PVR Information about the receiving device (pvrinput only) */
  cString           m_DeviceString;                 /*!> The name of the receiving device */
  bool              m_startup;
  bool              m_requestStreamChange;
  uint32_t          m_scanTimeout;                  /*!> Channel scanning timeout (in seconds) */
  cTimeMs           m_last_tick;
  bool              m_SignalLost;
  cMutex            m_FilterMutex;
  int               m_LanguageIndex;
  eStreamType       m_LangStreamType;
  cLiveQueue*       m_Queue;

protected:
  virtual void Action(void);
  void RequestStreamChange();

public:
  cLiveStreamer(uint32_t timeout = 0);
  virtual ~cLiveStreamer();

  void Activate(bool On);

  bool StreamChannel(const cChannel *channel, int priority, cxSocket *Socket, cResponsePacket* resp);
  bool IsReady();
  bool IsStarting() { return m_startup; }
  void SetLanguage(int lang, eStreamType streamtype = stAC3);

};

#endif  // XVDR_RECEIVER_H

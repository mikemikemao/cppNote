#ifndef RTSP_COMMON_H
#define RTSP_COMMON_H

class RtspCom
{
public:
	RtspCom() = delete;
	static constexpr int RTSP_PORT = 554;
	static constexpr int MIN_PORT = 62000;
	static constexpr int MAX_PORT = 65000;
};

#endif
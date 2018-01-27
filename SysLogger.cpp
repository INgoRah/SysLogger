#include <SysLogger.h>
#include <time.h>

size_t SysLogger::write(const uint8_t *buffer, size_t size) {
	static String s;
	Serial.write(buffer, size);
	if (buffer[size-1] != '\n')
		Serial.println();
	if (_ip != 0)
		_udp.beginPacket(_ip, 514);
	s = String((char*)buffer);
	if (_bufMax) {
		char s_tm[9];
		_now = time(nullptr);
		_tm = localtime(&_now);
		sprintf(s_tm, "%02d:%02d:%02d",
			_tm->tm_hour, _tm->tm_min, _tm->tm_sec);
		_last = _idx;
		_log[_idx++] = String(s_tm) + " " + s;
		if (_idx == _bufMax)
			_idx = 0;
	}
	if (_ip != 0) {
		s = "<5> " + s;
		_udp.write(s.c_str(), s.length());
		_udp.endPacket();
	}
	return size;
}

/* initialize the syslogger to an syslog server with its IP */
void SysLogger::init(IPAddress ip) {
	_ip = ip;
	_udp.begin(514);
}

/* Initialize the logger to only log into a circular buffer of
 * logBufSize entries */
void SysLogger::init(int logBufSize) {
	int i;
	_now = time(nullptr);
	_tm = localtime(&_now);

	_log = new String[logBufSize];
	for (i = 0; i < logBufSize; i++) {
		_log[i] = String("");
	}
	_bufMax = logBufSize;
}

/* Initialize the syslogger to an syslog server with its IP and
 * to log into a circular buffer of
 * logBufSize entries */
void SysLogger::init(IPAddress ip, int logBufSize) {
    init (ip);
    init (logBufSize);
}

/* returns an array of log string entries if a buffer size
 * was given in the init call. 
 * The caller must provide a buffer of the maximum size
 * specified in init
 */
int SysLogger::read(String* log)
{
	int i = _idx + 1;
	int j = 0, u = 0;

	while (j < _bufMax) {
		if (_log[i].length() > 0)
			log[u++] = _log[i];
		if (i >= _bufMax)
			i = 0;
		else
			i++;
		j++;
	}
	return u;
}

const String SysLogger::last()
{
	int i;
	if (_last == -1)
		return "";
	i = _last;
	_last = -1;

	return _log[i];
}

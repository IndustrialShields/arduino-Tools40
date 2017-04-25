#ifndef __SimplePacket_H__
#define __SimplePacket_H__

#include <Arduino.h>

class SimplePacket {
	public:
		explicit SimplePacket();
		virtual ~SimplePacket();

	public:
		// Packet functions
		void clear();
		void setSource(uint8_t source);
		void setDestination(uint8_t destination);
		void setType(uint8_t type);

		uint8_t getSource() const;
		uint8_t getDestination() const;
		uint8_t getType() const;

		// Payload (data) functions
		bool setData(bool data);
		bool setData(char data);
		bool setData(unsigned char data);
		bool setData(int data);
		bool setData(unsigned int data);
		bool setData(long data);
		bool setData(unsigned long data);
		bool setData(double data);
		bool setData(const String &data);
		bool setData(const char *data);
		bool setData(const void *data, uint8_t len);

		bool getBool() const;
		char getChar() const;
		unsigned char getUChar() const;
		int getInt() const;
		unsigned int getUInt() const;
		long getLong() const;
		unsigned long getULong() const;
		double getDouble() const;
		const char *getString() const;
		const void *getData() const;
		const void *getData(uint8_t &len) const;

		uint8_t getDataLength() const;

	private:
		bool init(uint8_t len);

	private:
		uint8_t _source;
		uint8_t _destination;
		uint8_t _type;
		uint8_t _len;
		uint8_t *_data;
};

#endif // __SimplePacket_H__

#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/StreamSocket.h"
#include <iostream>

using Poco::Event;
using Poco::Exception;
using Poco::UInt16;
using Poco::Net::StreamSocket;
using Poco::Net::TCPServer;
using Poco::Net::TCPServerConnection;
using Poco::Net::TCPServerConnectionFactory;
using Poco::Net::TCPServerConnectionFactoryImpl;

#define max_len 256

namespace
{
	class ClientConnection : public TCPServerConnection
	{
	public:
		ClientConnection(const StreamSocket &s) : TCPServerConnection(s)
		{
		}

		void run()
		{
			StreamSocket &ss = socket();
			try
			{
				char err_len[] = "Error length of string. The line is not to exceed 255 symbols.\n\0";
				char buffer[256] = "Welcome to POCO TCP server. Enter you string:\n";
				ss.sendBytes(buffer, strlen(buffer));
				clear_str(buffer);
				int n = ss.receiveBytes(buffer, max_len + 2);
				while (n > 0)
				{
					if (strlen(buffer) > max_len)
					{
						ss.sendBytes(err_len, strlen(err_len));
						poco_bugcheck_msg(err_len);
					}
					else
					{
						reverse_str(buffer);
						ss.sendBytes(buffer, strlen(buffer));
					}
					clear_str(buffer);
					n = ss.receiveBytes(buffer, max_len + 2);
				};
			}
			catch (Exception &exc)
			{
				std::cerr << exc.displayText() << std::endl;
			}
		}

	private:
		void clear_str(char str[])
		{
			for (int i = 0; i < max_len; i++)
				str[i] = '\0';
		}

	private:
		void reverse_str(char str[])
		{
			if ((strlen(str) - 2) > 0)
			{
				for (int i = 0; i < ((strlen(str) - 2) / 2); i++)
				{
					char ch = '\0';
					ch = str[i];
					str[i] = str[strlen(str) - 3 - i];
					str[strlen(str) - 3 - i] = ch;
				}
			}
		}
	};

	typedef TCPServerConnectionFactoryImpl<ClientConnection> TCPFactory;

	Event terminator;
}

int main(int argc, char **argv)
{
	try
	{
		Poco::UInt16 port = 28888;
		TCPServer srv(new TCPFactory(), port);
		srv.start();

		std::cout << "TCP server listening on port " << port << '.'
				  << std::endl
				  << "Press Ctrl-C to quit." << std::endl;

		terminator.wait();
	}
	catch (Exception &exc)
	{
		std::cerr << exc.displayText() << std::endl;
		return 1;
	}

	return 0;
}

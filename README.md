# WebSocket Trading Client for Deribit API

This project is a high-performance WebSocket-based trading client built in C++. It interfaces with the Deribit API to facilitate real-time trading operations, including placing orders, fetching order book data, checking positions, subscribing to channels, etc.

### Key Features
- Authenticate and interact with Deribit API using WebSocket.
- Support for place order, cancel order, modify order, get orderbook, view positions, subscribe to symbol.
- Command-line interface for interactive trading operations.
- JSON-based message exchange with structured logging and error handling.

## Dependencies

The following libraries are required for compilation and execution:

1. WebSocketpp: For handling WebSocket connections.
2. Boost: Required by WebSocket++ for various utilities.
3. RapidJSON: For fast JSON parsing and serialization of the response messages.
4. OpenSSL: For establishing secure connections using SSL/TLS.
5. pthread: Provides multi-threading support in C++.

Websocketpp is a header only library, so git clone the repository. Install other libraries and compile trade.cpp as:
```g++-13 -std=c++11 -I path/to/websocketpp -I /path/to/boost/include -I /path/to/rapidjson/include -I /path/to/openssl@3/include -L/path/to/openssl@3/lib -lssl -lcrypto -lpthread trade.cpp```

A menu is printed to the terminal which shows us the available option. Run ```tail -f output.txt``` in another terminal instance to view the reply messages. In case any error is encountered, the error is printed in the main terminal and detailed response is present in the other terminal.

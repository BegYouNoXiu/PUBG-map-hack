#pragma once
#pragma comment ( lib, "libcurl_a.lib" )
#include <curl/curl.h>
#include "json.hpp"
#include "Types.h"
#include <iostream>
#include <queue> 



class CURLWrapper
{
public:

	// Constructor of this wrapper. Also initializes the CURL.
	CURLWrapper()
	{
		curl_global_init(CURL_GLOBAL_ALL);
		m_curl = curl_easy_init();
		
		struct curl_slist *headers = NULL;
		
		headers = curl_slist_append(headers, "Content-Type: application/json");

		curl_easy_setopt(m_curl, CURLOPT_VERBOSE, 0L);
		curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(m_curl, CURLOPT_URL, "http://localhost:1531/");
		curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "POST");
		// curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 30L);
		curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data);
	
		curl_easy_setopt(m_curl, CURLOPT_NOSIGNAL, 1);

		curl_easy_setopt(m_curl, CURLOPT_TCP_KEEPALIVE, 1L);
		/* keep-alive idle time to 120 seconds */
		curl_easy_setopt(m_curl, CURLOPT_TCP_KEEPIDLE, 120L);
		/* interval time between keep-alive probes: 60 seconds */
		curl_easy_setopt(m_curl, CURLOPT_TCP_KEEPINTVL, 60L);


	}
	static CURLWrapper* getInstance()
	{
		static CURLWrapper instance;
		return &instance;
	}

	void transferToOutnet() 
	{
		curl_easy_setopt(m_curl, CURLOPT_URL, "http://148.66.1.157:1531/");
	}
	

	~CURLWrapper()
	{
		curl_easy_cleanup(m_curl);
	}

	bool getReadyState()
	{
		if (m_curl) return true;
		return false;
	}

	static size_t write_data(void* w_buffer, size_t w_size, size_t w_nmemb, void* w_userp)
	{
		return w_size * w_nmemb;
	}

	void putQueue(std::string &w_data) {

		if (espDataQueue.size() > 10) {
			espDataQueue.pop();
		}
		char* temp_c;
		const int len = w_data.length();
		temp_c = new char[len + 1];
		
		strcpy_s(temp_c, len + 1, w_data.c_str());
	
		//_MakeStringcode(temp_c, key);
		espDataQueue.push(temp_c);
	

	}
	
	void _MakeStringcode(char  *pstr, int *pkey) {
		int len = strlen(pstr);
		for (int i = 0; i < len; i++) {
			/*if (*(pstr + i) == '{' || *(pstr + i) == ':' || *(pstr + i) == '}') {
				continue;
			}*/
			*(pstr + i) = (*(pstr + i) ^ pkey[i % 5]);
		}
		
	}

	void _CuteStringcode(char *pstr, int *pkey) {
		int len = strlen(pstr);
		for (int i = 0; i<len; i++)
			*(pstr + i) = (*(pstr + i) ^ pkey[i % 5]);

	}
	
	void sendData()
	{	
		try
		{	
			if (espDataQueue.empty()) {
				return;
			}
			char* temp_c = espDataQueue.front();
			//std::string w_data = espDataQueue.front();
			espDataQueue.pop();
			//char stackChar[] = { temp_c };
			//_CuteStringcode(temp_c, key);
			//std::cout <<"data:" <<w_data << std::endl;
			curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, temp_c);
			curl_easy_perform(m_curl);

			delete temp_c;

		}
		catch (std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
		
	}

private:
	CURL* m_curl;			// the curl interface
	json m_data;			// json data wrapper
	std::queue<char*> espDataQueue;
    int key[5] = { 1,3,4,6,7 };//¼ÓÃÜ×Ö·û  
};
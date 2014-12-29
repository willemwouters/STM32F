#include "stdio.h"
#include "ctype.h"
#include "string.h"
#include "stdlib.h"

#include "mdns.h"
#include "mxchipWNet.h"

typedef struct
{
	char* hostname;
	char* instance_name;
	char* service_name;
  char* txt_att;
	u16	port;
	char		instance_name_suffix[4]; // This variable should only be modified by the DNS-SD library
} dns_sd_service_record_t;

#define SERVICE_QUERY_NAME  "_services._dns-sd._udp.local."
#define SERVICE_HTTP      	"_http._tcp.local."

#define INSTANCE_NAME      	"MXCHIP_316x._http._tcp.local."
#define HOST_NAME 					"EMW316x.local."
#define SERVICE_PORT      	80


#define TXT_ATT 						"Vendor=MXCHIP.Version=1.protocol=tcp.port=8080." 


extern u32 my_ip_addr;

static dns_sd_service_record_t*   available_services	= NULL;
static u8	available_service_count;


static void dns_get_next_question( dns_message_iterator_t* iter, dns_question_t* q, dns_name_t* name );
static int dns_compare_name_to_string( dns_name_t* name, const char* string );
static char* dns_convert_name_to_string( dns_name_t* name );
static void dns_create_message( dns_message_iterator_t* message, u16 size );
static void dns_write_header( dns_message_iterator_t* iter, u16 id, u16 flags, u16 question_count, u16 answer_count, u16 authorative_count );
static void dns_write_record( dns_message_iterator_t* iter, const char* name, u16 record_class, u16 record_type, uint32_t ttl, u8* rdata );
static void mdns_send_message(int fd, dns_message_iterator_t* message );
static void dns_free_message( dns_message_iterator_t* message );
static void mdns_process_query(int fd, dns_name_t* name, dns_question_t* question, dns_message_iterator_t* source );
static void dns_write_record( dns_message_iterator_t* iter, const char* name, u16 record_class, u16 record_type, uint32_t ttl, u8* rdata );
static char* dns_read_name( dns_message_iterator_t* iter );
static void dns_write_uint16( dns_message_iterator_t* iter, u16 data );
static void dns_write_uint32( dns_message_iterator_t* iter, uint32_t data );
static void dns_write_bytes( dns_message_iterator_t* iter, u8* data, u16 length );
static u16 dns_read_uint16( dns_message_iterator_t* iter );
static uint32_t dns_read_uint32( dns_message_iterator_t* iter);
static void dns_skip_name( dns_message_iterator_t* iter );
static void dns_write_name( dns_message_iterator_t* iter, const char* src );


static char *__strdup(char *src)
{
	int len;
	char *dst;
    
  if (src == NULL)
    return NULL;

  if (src[0] == 0)
    return NULL;

  len = strlen(src) + 1;
  dst = (char*)malloc(len);
  if (dst) 
    memcpy(dst, src, len);
  return dst;
}

void process_dns_questions(int fd, dns_message_iterator_t* iter )
{
  dns_name_t name;
  dns_question_t question;
  dns_message_iterator_t response;
  int a = 0;
  int question_processed;
  u32 myip = htonl(my_ip_addr);

  memset( &response, 0, sizeof(dns_message_iterator_t) );
    
  for ( a = 0; a < htons(iter->header->question_count); ++a )
  {
    dns_get_next_question( iter, &question, &name );
    question_processed = 0;
    switch ( question.question_type ){
      case RR_TYPE_PTR:
        if ( available_services != NULL ){
          // Check if its a query for all available services
          if ( dns_compare_name_to_string( &name, SERVICE_QUERY_NAME ) ){
            int b = 0;
            printf("Recv a SERVICE QUERY request.\r\n");
            dns_create_message( &response, 512 );
						dns_write_header(&response, iter->header->id, 0x8400, 0, available_service_count, 0 );          
            for ( b = 0; b < available_service_count; ++b ){
              dns_write_record( &response, SERVICE_QUERY_NAME, RR_CLASS_IN, RR_TYPE_PTR, 300, (u8*) available_services[b].service_name );
            }
            mdns_send_message(fd, &response );
            dns_free_message( &response );
            question_processed = 1;
          }
          // else check if its one of our records
          else {
            int b = 0;
            for ( b = 0; b < available_service_count; ++b ){
              if ( dns_compare_name_to_string( &name, available_services[b].service_name )){
                // Send the PTR, TXT, SRV and A records
                printf("Recv a SERVICE Detail request.\r\n");
                dns_create_message( &response, 512 );
                dns_write_header( &response, iter->header->id, 0x8400, 0, 4, 0 );
                dns_write_record( &response, available_services[b].service_name, RR_CLASS_IN, RR_TYPE_PTR, 300, (u8*) available_services[b].instance_name );
                dns_write_record( &response, available_services[b].instance_name, RR_CACHE_FLUSH|RR_CLASS_IN, RR_TYPE_TXT, 300, (u8*) available_services->txt_att );
                dns_write_record( &response, available_services[b].instance_name, RR_CACHE_FLUSH|RR_CLASS_IN, RR_TYPE_SRV, 300, (u8*) &available_services[b]);
                dns_write_record( &response, available_services[b].hostname, RR_CACHE_FLUSH|RR_CLASS_IN, RR_TYPE_A, 300, (u8*) &myip);
                mdns_send_message(fd, &response );
                dns_free_message( &response );
                question_processed = 1;
              }
            }
          }
        }
        break;
      }
      if (!question_processed ){
        printf("Recv a mDNS request.\r\n");
        mdns_process_query(fd, &name, &question, iter);
      }
    }
}

static void mdns_process_query(int fd, dns_name_t* name, 
                               dns_question_t* question, dns_message_iterator_t* source )
{
  dns_message_iterator_t response;
  u32 myip = htonl(my_ip_addr);

  memset( &response, 0, sizeof(dns_message_iterator_t) );

  switch ( question->question_type )
  {
    case RR_QTYPE_ANY:
    case RR_TYPE_A:
    if ( dns_compare_name_to_string( name, available_services->hostname ) ){				
      dns_create_message( &response, 256 );
      dns_write_header( &response, source->header->id, 0x8400, 0, 1, 0 );
      dns_write_record( &response, available_services->hostname, RR_CLASS_IN | RR_CACHE_FLUSH, RR_TYPE_A, 300, (u8*)&myip);
      mdns_send_message(fd, &response );
      dns_free_message( &response );
    }
    break;
  }
}


static void dns_get_next_question( dns_message_iterator_t* iter, dns_question_t* q, dns_name_t* name )
{
	// Set the name pointers and then skip it
	name->start_of_name   = (u8*) iter->iter;
	name->start_of_packet = (u8*) iter->header;
	dns_skip_name( iter );

	// Read the type and class
	q->question_type  = dns_read_uint16( iter );
	q->question_class = dns_read_uint16( iter );
}


void mdns_handler(int fd, u8* pkt, int pkt_len)
{
  dns_message_iterator_t iter;

  iter.header = (dns_message_header_t*) pkt;
  iter.iter   = (u8*) iter.header + sizeof(dns_message_header_t);

  // Check if the message is a response (otherwise its a query)
  if ( ntohs(iter.header->flags) & DNS_MESSAGE_IS_A_RESPONSE )
  {
  }
  else
  {
    process_dns_questions(fd, &iter );
  }
}

static int dns_compare_name_to_string( dns_name_t* name, const char* string )
{
	u8 section_length;
	int finished = 0;
	int result   = 1;
	u8* buffer 	  = name->start_of_name;

	while ( !finished )
	{
		// Check if the name is compressed. If so, find the uncompressed version
		while ( *buffer & 0xC0 )
		{
			u16 offset = ( *buffer++ ) << 8;
			offset += *buffer;
			offset &= 0x3FFF;
			buffer = name->start_of_packet + offset;
		}

		// Compare section
		section_length = *( buffer++ );
		if ( strncmp( (char*) buffer, string, section_length ) )
		{
			result	 = 0;
			finished = 1;
		}
		string += section_length + 1;
		buffer += section_length;

		// Check if we've finished comparing
		if ( *buffer == 0 || *string == 0 )
		{
			finished = 1;
			// Check if one of the strings has more data
			if ( *buffer != 0 || *string != 0 )
			{
				result = 0;
			}
		}
	}

	return result;
}

static char* dns_convert_name_to_string( dns_name_t* name )
{
	// Make a iterator structure from the DNS_Name
	dns_message_iterator_t iter;
	iter.header = (dns_message_header_t*) name->start_of_packet;
	iter.iter	= name->start_of_name;
	return dns_read_name( &iter );
}

static void dns_create_message( dns_message_iterator_t* message, u16 size )
{
	message->header = (dns_message_header_t*) malloc( size );
	if ( message->header == NULL )
	{
		return;
	}

	message->iter = (u8*) message->header + sizeof(dns_message_header_t);
}

static void dns_free_message( dns_message_iterator_t* message )
{
	free(message->header);
	message->header = NULL;
}

static void dns_write_string( dns_message_iterator_t* iter, const char* src )
{
	u8* segment_length_pointer;
	u8  segment_length;

	while ( *src != 0 && *src != 0xC0)
	{
		/* Remember where we need to store the segment length and reset the counter*/
		segment_length_pointer = iter->iter++;
		segment_length = 0;

		/* Copy bytes until '.' or end of string*/
		while ( *src != '.' && *src != 0 && *src != 0xC0)
		{
		    if (*src == '/')
                src++; // skip '/'
            
			*iter->iter++ = *src++;
			++segment_length;
		}

		/* Store the length of the segment*/
		*segment_length_pointer = segment_length;

		/* Check if we stopped because of a '.', if so, skip it*/
		if ( *src == '.' )
		{
			++src;
		}
        
	}

    if (*src == 0xC0) { // compress name
        *iter->iter++ = *src++;
        *iter->iter++ = *src++;
    } else {
        /* Add the ending null */
	    *iter->iter++ = 0;
    }
}


static void dns_write_header( dns_message_iterator_t* iter, u16 id, u16 flags, u16 question_count, u16 answer_count, u16 authorative_count )
{
	memset( iter->header, 0, sizeof(dns_message_header_t) );
	iter->header->id				= htons(id);
	iter->header->flags 			= htons(flags);
	iter->header->question_count	= htons(question_count);
	iter->header->name_server_count = htons(authorative_count);
	iter->header->answer_count		= htons(answer_count);
}


static void dns_write_record( dns_message_iterator_t* iter, const char* name, u16 record_class, u16 record_type, uint32_t ttl, u8* rdata )
{
	u8* rd_length;
	u8* temp_ptr;

	/* Write the name, type, class, TTL*/
	dns_write_name	( iter, name );
	dns_write_uint16( iter, record_type );
	dns_write_uint16( iter, record_class );
	dns_write_uint32( iter, ttl );

	/* Keep track of where we store the rdata length*/
	rd_length	= iter->iter;
	iter->iter += 2;
	temp_ptr	= iter->iter;

	switch ( record_type )
	{
		case RR_TYPE_A:
			dns_write_bytes( iter, rdata, 4 );
			break;
			
		case RR_TYPE_PTR:
		case RR_TYPE_TXT:
			dns_write_name( iter, (const char*) rdata );
			break;

		case RR_TYPE_SRV:
			/* Set priority and weight to 0*/
			dns_write_uint16( iter, 0 );
			dns_write_uint16( iter, 0 );

			/* Write the port*/
			dns_write_uint16( iter, ( (dns_sd_service_record_t*) rdata )->port );

			/* Write the hostname*/
			dns_write_string( iter, ( (dns_sd_service_record_t*) rdata )->hostname );
			break;
		default:
			break;
	}
	// Write the rdata length
	rd_length[0] = ( iter->iter - temp_ptr ) >> 8;
	rd_length[1] = ( iter->iter - temp_ptr ) & 0xFF;
}

static void mdns_send_message(int fd, dns_message_iterator_t* message )
{
	printf("Send a mdns respond!\r\n");
  send(fd, message->header, message->iter - (u8*)message->header, 0);
}

static char* dns_read_name( dns_message_iterator_t* iter )
{
	u16	   length = 0;
	const u8* buffer_backup = 0; // This keeps a backup of the buffer pointer in case we need to jump around the packet due to compressed names
	u8*	   buffer = iter->iter;
	char*		   string_iter;
	char*		   string;
	u16	   offset;
	u8 	   section_length;

	// Find out how long the string is
	while ( *buffer != 0 )
	{
		// Check if the name is compressed
		if ( *buffer & 0xC0 )
		{
			u16 offset = ( *buffer++ ) << 8;
			offset += *buffer;
			offset &= 0x3FFF;
			buffer = (u8*) iter->header + offset;
		}
		else
		{
			length += *buffer + 1; // +1 for the '.', unless its the last section in which case its the ending null character
			buffer += *buffer + 1;
		}
	}

	// Allocate memory for the string
	string = (char*) malloc( length );
	if ( string == NULL )
	{
		return NULL;
	}

	string_iter = string;

	buffer = iter->iter;
	while ( *buffer != 0 )
	{
		// Check if the name is compressed. If so, find the uncompressed version
		if ( *buffer & 0xC0 )
		{
			// Make a backup of the buffer pointer if it's not already set
			if ( buffer_backup == 0 )
			{
				buffer_backup = buffer + 2;
			}
			// Follow the offsets to an uncompressed name
			while ( *buffer & 0xC0 )
			{
				offset	= ( *buffer++ ) << 8;
				offset += *buffer;
				offset &= 0x3FFF;
				buffer	= (u8*) iter->header + offset;
			}
		}

		// Copy the section of the name
		section_length = *( buffer++ );
		memcpy( string_iter, buffer, section_length );
		string_iter += section_length;
		buffer		+= section_length;

		// Add a '.' if the next section is valid, otherwise terminate the string
		if ( *buffer != 0 )
		{
			*string_iter++ = '.';
		}
		else
		{
			*string_iter = 0;
		}
	}

	// If required, restore the buffer pointer to the correct value
	if ( buffer_backup != 0 )
	{
		iter->iter = (u8*) buffer_backup;
	}
	else
	{
		iter->iter = buffer;
	}

	return string;
}

static void dns_write_uint16( dns_message_iterator_t* iter, u16 data )
{
	// We cannot assume the u8 alignment of iter->iter so we can't just typecast and assign
	iter->iter[0] = data >> 8;
	iter->iter[1] = data & 0xFF;
	iter->iter += 2;
}

static void dns_write_uint32( dns_message_iterator_t* iter, uint32_t data )
{
	iter->iter[0] = data >> 24;
	iter->iter[1] = data >> 16;
	iter->iter[2] = data >> 8;
	iter->iter[3] = data & 0xFF;
	iter->iter += 4;
}

static void dns_write_bytes( dns_message_iterator_t* iter, u8* data, u16 length )
{
	int a = 0;

	for ( a = 0; a < length; ++a )
	{
		iter->iter[a] = data[a];
	}
	iter->iter += length;
}

static u16 dns_read_uint16( dns_message_iterator_t* iter )
{
	u16 temp = (u16) ( *iter->iter++ ) << 8;
	temp += (u16) ( *iter->iter++ );
	return temp;
}

static uint32_t dns_read_uint32( dns_message_iterator_t* iter )
{
	uint32_t temp = (uint32_t) ( *iter->iter++ ) << 24;
	temp += (uint32_t) ( *iter->iter++ ) << 16;
	temp += (uint32_t) ( *iter->iter++ ) << 8;
	temp += (uint32_t) ( *iter->iter++ );

	return temp;
}

static void dns_skip_name( dns_message_iterator_t* iter )
{
	while ( *iter->iter != 0 )
	{
		// Check if the name is compressed
		if ( *iter->iter & 0xC0 )
		{
			iter->iter += 1; // Normally this should be 2, but we have a ++ outside the while loop
			break;
		}
		else
		{
			iter->iter += (uint32_t) *iter->iter + 1;
		}
	}
	// Skip the null u8
	++iter->iter;
}

static void dns_write_name( dns_message_iterator_t* iter, const char* src )
{
	dns_write_string( iter, src );
}


void mdns_init()
{
	int len;
	available_service_count = 1;
	available_services = (void *)malloc(sizeof(dns_sd_service_record_t) * 1);

	available_services->hostname = (char*)__strdup(HOST_NAME);
	available_services->instance_name = (char*)__strdup(HOST_NAME);
  len = strlen(INSTANCE_NAME);
  available_services->instance_name = (char*)malloc(len+3);//// 0xc00c+\0
  memcpy(available_services->instance_name, INSTANCE_NAME, len);
  available_services->instance_name[len]= 0xc0;
  available_services->instance_name[len+1]= 0x0c;
  available_services->instance_name[len+2]= 0;
	available_services->service_name = SERVICE_HTTP;
  available_services->txt_att = (char*)__strdup(TXT_ATT);
	available_services->port = SERVICE_PORT;	
}

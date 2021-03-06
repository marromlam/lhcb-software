#ifndef CHUNK_HEADERS_H_
#define CHUNK_HEADERS_H_

#include <sys/types.h>

/**
 * Maximum length of a file name in bytes.
 */
#define MAX_FILE_NAME 256
#define MAX_TRIGGER_TYPES 8
#define MAX_STAT_TYPES 14

#define PHYSIN    0
#define MBIASIN   1
#define LUMIIN    2
#define BEAMGASIN 3
#define RANDIN    4
#define PHYSEX    5
#define MBIASEX   6
#define LUMIEX    7
#define BEAMGASEX 8
#define RANDEX    9
#define LOWLUMI   10
#define MIDLUMI   11
#define HLT1IN    12
#define HLT1EX	  13
 
/**
 * Encapsulates a command that is accompanied by data to be written.
 * The data that accompanies this header is essentially what is
 * received by the MDFWriterNet GaudiAlgorithm in a single 
 * writeBuffer() call.
 */
struct cmd_chunk_header {
	__uint32_t seq_num; /**< The sequence number of this command */
	__uint32_t size;    /**< The size of the chunk data */
	__uint64_t offset;  /**< The offset of this data in the file */
} __attribute__((__packed__));

/**
 * Encapsulates a command that indicates that a new file must be created.
 * There is no data with this command header.
 */
struct cmd_start_header {
	__uint32_t seq_num; /**< The sequence number of this command */
} __attribute__((__packed__));

/**
 * Sent with a command to indicate a file is being closed.
 */
struct cmd_stop_header {
	__uint32_t seq_num;        /**< The sequence number of this command */
        __uint64_t size;           /**< The total size of the file */

} __attribute__((__packed__));

/**
 * PDU format of a close command.
 * It actually won't be sent to the writerd because this one does not care about these information.
 * It is just stored in the close command for failover purpose.
 */
struct cmd_stop_pdu {
	unsigned char md5_sum[16]; /**< The MD5 checksum calculated at client */
	__uint32_t adler32_sum;    /**< The Adler checksum calculated at client */
        __uint32_t events;         /**< The number of events stored in the file */
        __uint32_t physStat;       /**< The number of physics events */
        __uint32_t trgEvents[MAX_TRIGGER_TYPES]; /**< The triggers in the Odin banks */
        __uint32_t statEvents[MAX_STAT_TYPES];   /**< Exclusive and inclusive counters of some routing bits */
} __attribute__((__packed__));


#define CMD_OPEN_FILE	0x01  /**< The command code for opening a file */
#define	CMD_CLOSE_FILE	0x02  /**< The command code for closing a file */
#define CMD_WRITE_CHUNK	0x03  /**< The command code for writing a chunk to a file */

/**
 * A header for a command that specifies an operation.
 * This header encapsulates one of cmd_start_header, cmd_stop_header ,or
 * cmd_chunk_header based on the command code. In other words, each time
 * a command is sent from the MDFWriterNet job, this structure is received
 * over the socket.
 */
struct cmd_header {
	__uint32_t cmd;     /**< The command code (CMD_XXX) */
	__uint32_t run_no;  /**< The run number associated with the command */
	char file_name[MAX_FILE_NAME]; /**< The file number associated with the command */
	union {
		struct cmd_start_header 	start_data;
		struct cmd_stop_header 		stop_data;
		struct cmd_chunk_header		chunk_data;
	} data;  /**< A start, stop or chunk header, based on the command code */
} __attribute__((__packed__));


/**
 * Encapsulates an acknowledgement sent back to the MDFWriter job.
 * This struct is sent out to the sender to acknowledge
 * the range of sequence numbers has been processed. Note that a single
 * acknowledgement does not necessarily correspond to a single command.
 * In case of CMD_WRITE_CHUNK commands, several write commands may be
 * acknowledged by sending a single ack_header.
 */
struct ack_header {
	/**
	 * The run number of the file whose commands are being
	 * acknowledged.
	 */
	__uint32_t run_no;

	/**
	 * The sequence number of the earliest command that is being acknowledged.
	 */
	__uint32_t min_seq_num;

	/**
	 * The sequence number of the laast command that is being acknowledged.
	 */
	__uint32_t max_seq_num;
} __attribute__((__packed__));

#endif

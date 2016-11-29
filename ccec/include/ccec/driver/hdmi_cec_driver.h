/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/



/**
* @defgroup hdmicec
* @{
* @defgroup ccec
* @{
**/


#ifndef HDMI_CEC_DRIVER_H_
#define HDMI_CEC_DRIVER_H_
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Error Codes.
 */
enum {
	HDMI_CEC_IO_SUCCESS = 0,
    HDMI_CEC_IO_SENT_AND_ACKD = 1, 
    HDMI_CEC_IO_SENT_BUT_NOT_ACKD,
    HDMI_CEC_IO_SENT_FAILED,
    HDMI_CEC_IO_INVALID_STATE,
    HDMI_CEC_IO_INVALID_ARGUMENT,
    HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE,
    HDMI_CEC_IO_GENERAL_ERROR,
};

/*
 * CEC Driver should call this function whenever this is a complete CEC packet received.
 * Upon each callback, only 1 complete CEC packet should be contained in the buffer. 
 */

typedef void (*HdmiCecRxCallback_t)(int handle, void *callbackData, unsigned char *buf, int len);

/*
 * CEC Driver should call this function to report the staus of the latest transmit.
 */

typedef void (*HdmiCecTxCallback_t)(int handle, void *callbackData, int result);
/**
 * @brief opens an instance of CEC driver.  
 * This function should be call once before the functions in this API can be used.
 *
 * @param [in]  :  None.
 * @param [out] :  handle used by application to uniquely identify the driver instance. 
 *
 * @return Error Code:  If error code is returned, the open is failed.
 */
int HdmiCecOpen(int *handle);

/**
 * @brief close an instance of CEC driver.  
 * This function should close the currently opened driver instance.
 *
 * @param [in]  :  handle returned from the HdmiCecOpen() function.
 * @param [out] :  None 
 *
 * @return Error Code:  If error code is returned, the open is failed.
 */
int HdmiCecClose(int handle);

/**
 * @brief This API is **DEPRECATED** due to possible race conditions competing
 * for a logical address.
 *
 * Set the Logical Addresses claimed by host device.
 * This function sets multiple logical addresses used by the host.  The host
 * has claimed these logical address through the Discovery process.  Once 
 * set, the host shall receive all CEC packets destined to these addresses.
 *
 * Once the driver is opened, the host shall receive all broadcast messages 
 * regardless the Logical addresses.
 * 
 * when there is no logical address set, the host shall only receive broadcast
 * messages.
 *
 * @param [in]  :  handle returned from the HdmiCecOpen() function.
 * @param [in]  :  logicalAddresses set or replace addresses claimed by host.
 *                 A null value clears the current list. 
 * @param [in]  :  number of logical addresses.  If any logical address in the list
 *                 cannot be set, none of them should be set upon return. Success
 *                 return indicates that all addresses in the list are set.
 * @param [out] :  None 
 *
 * @return Error Code:  If error code is returned, the set is failed.
 */
int HdmiCecSetLogicalAddress(int handle, int *logicalAddresses, int num);

/**
 * @brief Add one Logical Addresses to be used by host device.
 *
 * This function can block until the intended logical address is secured by
 * the driver.
 *
 * In driver implementation, this API would trigger driver sending a POLL
 * CEC packet to the CEC Bus,
 *
 * Packet::HeaderBlock::Initiator   =  Requested LogicalAddress.
 * Packet::HeaderBlock::Destination =  Requested LogicalAddress.
 * Packet::DataBlock   				=  Empty.
 *
 * The function shall return HDMI_CEC_IO_SUCCESS if the POLL message is sent
 * successfully and not ACK'd by any device on the bus. From this point on the
 * driver shall forward all received messages with destination being the acquired
 * logical address. Driver should ACK all POLL messsges destined to this logical
 * address.
 *
 * The function shall return HDMI_CEC_IO_LOGICALADDRESS_UNAVAILABLE if the POLL
 * message is sent and ACK'd by a device on the bus.
 *
 * The function shall return relevant error code if the POLL message is not sent
 * successfully.
 *
 *
 * @param [in]  :  handle returned from the HdmiCecOpen() function.
 * @param [in]  :  logicalAddresses to be acquired.
 * @param [out] :  None
 *
 * @return Error Code: See above.
 */
int HdmiCecAddLogicalAddress(int handle, int logicalAddresses);

/**
 * @brief Clear the Logical Addresses claimed by host device.
 *
 * This function release the previously acquired logical address.  Once
 * released, driver should not ACK any POLL message destined to the
 * released address.
 *
 * @param [in]  :  handle returned from the HdmiCecOpen() function.
 * @param [in]  :  logicalAddresses to be released.
 * @param [out] :  None
 *
 * @return Error Code:  see above.
 */
int HdmiCecRemoveLogicalAddress(int handle, int logicalAddresses);

/**
 * @brief Get the Logical Addresses claimed by host device.  
 *
 * This function gets multiple logical addresses set by the host.  It also means
 * that the driver is actively pick up unicast messages destined to the set of
 * logical addresses returned.
 *
 * @param [in]     :  handle returned from the HdmiCecOpen() function.
 * @param [out]    :  logicalAddresses list of addresses currently set. 
 * @param [in/out] :  size of the logicalAddress list.  Returned number is the total
 *                    number of addresses currently set. This can be larger than the
 *                    input value. An returned value 0 indicate that no address is set. 
 *
 * @return Error Code:  If error code is returned, the get is failed.
 */
/*int HdmiCecGetLogicalAddress(int handle, int *logicalAddresses, int *num);*/

/**
 * @brief Get the Logical Address obtained by the driver.
 *
 * This function get the logical address for the specified device type.
 *
 * @param [in]     :  handle returned from the HdmiCecOpen() function.
 * @param [in]     :  device type (tuner, record, playback etc.).
 * @param [out]    :  logical address acquired
 *
 * @return Error Code:  If error code is returned, the get is failed.
 */
int HdmiCecGetLogicalAddress(int handle, int devType,  int *logicalAddress);

/**
 * @brief Sets CEC packet Receive callback.  
 *
 * This function sets a callback function to be invoked for each packet arrival.   
 * The packet contained in the buffer is expected to follow this format:
 *
 * (ref <HDMI Specification 1-4> Section <CEC 6.1>)
 * 
 * complete packet = header block + data block;
 * header block = destination logical address (4-bit) + source address (4-bit)
 * data   block = opcode block (8-bit) + oprand block (N-bytes)                 
 *
 * |------------------------------------------------
 * | header block  |          data blocks          |
 * |------------------------------------------------
 * |3|2|1|0|3|2|1|0|7|6|5|4|3|2|1|0|7|6|5|4|3|2|1|0|  
 * |------------------------------------------------
 * | Dest  |  src  |  opcode block | operand block |
 * |------------------------------------------------
 *
 * when receiving, the returned buffer should not contain EOM and ACK bits.
 * 
 * When transmitting, it is driver's responsibility to insert EOM bit and ACK bit 
 * for each header or data block 
 *
 * When HdmiCecSetRxCallback is called, it replaces the previous set cbfunc and data
 * values.  Setting a value of (cbfunc=null) turns off the callback.
 *
 * This function should block if callback invocation is in progress.
 *
 * @param [in]     :  handle returned from the HdmiCecOpen(() function.
 * @param [in]     :  cbfunc to be invoked when a complete packet is received.
 * @param [in]     :  data, used when invoking callback function. 
 *
 * @return Error Code:  If error code is returned, the set is failed.
 */
int HdmiCecSetRxCallback(int handle, HdmiCecRxCallback_t cbfunc, void *data);

/**
 * @brief Sets CEC packet Transmit callback.  
 *
 * This function sets a callback function to be invoked once the async tranmit 
 * result is available. This is only necessary if application choose to transmit
 * the packet asynchronously.
 *
 * This function should block if callback invocation is in progress.
 *
 * @param [in]     :  handle returned from the HdmiCecOpen(() function.
 * @param [in]     :  cbfunc to be invoked when a complete packet is received.
 * @param [in]     :  data, used when invoking callback function. 
 *
 * @return Error Code:  If error code is returned, the set is failed.
 */
int HdmiCecSetTxCallback(int handle, HdmiCecTxCallback_t cbfunc, void *data);

/**
 * @brief Writes CEC packet onto bus.  
 *
 * This function writes a complete CEC packet onto the CEC bus and wait for ACK.
 * application should check for result when return value of the function is 0;
 *
 * The bytes in @param buf that is to be transmitted should follow the buffer
 * byte format required for receiving buffer. (See detailed description from 
 * HdmiCecSetRxCallback)
 *
 * @param [in]     :  handle returned from the HdmiCecOpen(() function.
 * @param [in]     :  buf contains a complete CEC packet.
 * @param [in]     :  len number of bytes in the packet.
 * @param [out]    :  result of the send. Possbile results are SENT_AND_ACKD, 
 *                    SENT_BUT_NOT_ACKD (e.g. no follower at the destionation),
 *                    SENT_FAILED (e.g. collision).
 *
 * @return Error Code:  If error code is returned, the transmit did not happen.
 */
int HdmiCecTx(int handle, const unsigned char *buf, int len, int *result);

/**
 * @brief Writes CEC packet onto bus asynchronously.
 *
 * This function writes a complete CEC packet onto the CEC bus but does not wait 
 * for ACK. The result will be reported via HdmiCecRxCallback_t if return value
 * of this function is 0.
 *
 * @param [in]     :  handle returned from the HdmiCecOpen(() function.
 * @param [in]     :  buf contains a complete CEC packet.
 * @param [in]     :  len number of bytes in the packet.
 *
 * @return Error Code:  If error code is returned, the transmit did not happen.
 */
int HdmiCecTxAsync(int handle, const unsigned char *buf, int len);
#ifdef __cplusplus
}
#endif
#endif


/** @} */
/** @} */

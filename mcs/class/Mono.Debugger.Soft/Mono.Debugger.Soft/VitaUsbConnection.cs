using System;
using System.IO.Ports;
using System.Threading;
using Mono.Debugger.Soft;

namespace Mono.Debugger.Soft
{

public class VitaUsbConnection : Connection
{
	SerialPort port;

	public VitaUsbConnection (string port_name) {
		port = new SerialPort (port_name);
        port.Parity = Parity.None;
        port.StopBits = StopBits.One;
        port.DataBits = 8;
        port.Handshake = Handshake.None;
		port.DtrEnable = true;
        port.RtsEnable = true;

		while (true) {
			try {
				port.Open ();
				break;
			} catch {
				/* Port not created yet */
			}
			Thread.Sleep (100);
		}
	}

	protected override int TransportReceive (byte[] buf, int buf_offset, int len) {

		while (true) {
			if (port.BytesToRead >= len) {
				//Console.WriteLine ("recv:");
				int res = port.Read (buf, buf_offset, len);
				//Console.WriteLine ("recv: " + len);
				return res;
			}
			Thread.Sleep (30);
		}
	}

	protected override int TransportSend (byte[] buf, int buf_offset, int len) {
		//Console.WriteLine ("send: " + len);
		port.Write (buf, buf_offset, len);
		return len;
	}

	protected override void TransportSetTimeouts (int send_timeout, int receive_timeout) {
	}

	protected override void TransportClose () {
		port.Close ();
	}
}

}
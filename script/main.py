import sys, serial, time

UART_COM_PORT = sys.argv[1]
UART_BAUDRATE_PORT = sys.argv[2]

ser = serial.Serial(UART_COM_PORT, UART_BAUDRATE_PORT, timeout = 3)

write_data = """\"Finance Minister Arun Jaitley Tuesday hit out at former RBI governor Raghuram Rajan 
for predicting that the next banking crisis would be triggered by MSME lending, saying postmortem is 
easier than taking action when it was required. Rajan, who had as the chief economist at IMF warned of 
impending financial crisis of 2008, in a note to a parliamentary committee warned against ambitious credit 
targets and loan waivers, saying that they could be the sources of next banking crisis. Government should 
focus on sources of the next crisis, not just the last one.* 

*In particular, government should refrain from setting ambitious credit targets or waiving loans. Credit 
targets are sometimes achieved by abandoning appropriate due diligence, creating the environment for future 
NPAs," Rajan said in the note." Both MUDRA loans as well as the Kisan Credit Card, while popular, have to be 
examined more closely for potential credit risk. Rajan, who was RBI governor for three years till September 
2016, is currently.\""""

def do_something():
    
    count = 0

    while 1:
        count += 1

        print("count: {}".format(count))

        # write data here
        print("len(write_data): {}".format(len(write_data)))
        print("write_data: {}\n".format(write_data))
        ser.write(bytes(write_data, 'utf-8'))

        # read data here
        read_data = ser.read(len(write_data)).decode('utf-8')
        print("len(read_data): {}".format(len(read_data)))
        print("read_data: {}\n".format(read_data))

        # wait for 2 sec.
        time.sleep(2)

if __name__ == "__main__":
    do_something()
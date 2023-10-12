while(True) :
    import serial.tools.list_ports
    import serial
    import pymongo
    from datetime import datetime
    import pytz

    ports = serial.tools.list_ports.comports()
    serialInst = serial.Serial()

    # ตั้งค่า MongoDB connection
    client = pymongo.MongoClient("mongodb://root:161145@4e46eeb321f6c7aa.sn.mynetname.net:27017")  # เปลี่ยน URL ของ MongoDB ตามที่คุณใช้
    db = client["project"]  # เปลี่ยนชื่อฐานข้อมูลตามที่คุณต้องการ
    collection = db["students"]  # เปลี่ยนชื่อคอลเลกชันตามที่คุณต้องการ


    serialInst.baudrate = 115200
    serialInst.port = "/dev/ttyAML1"
    serialInst.open()

    data1 = ""  # ตัวแปรสำหรับบรรทัดที่ 1
    data2 = ""  # ตัวแปรสำหรับบรรทัดที่ 2
    data3 = ""  # ตัวแปรสำหรับบรรทัดที่ 3

    count = 0  # ใช้เพื่อติดตามจำนวนบรรทัดที่อ่าน
    x = int(0)

    while count < 3:
        if serialInst.in_waiting:
            packet = serialInst.readline()
            decoded_packet = packet.decode('utf').rstrip('\n')
            count += 1
            if count == 1:
                data1 = decoded_packet
            elif count == 2:
                data2 = decoded_packet
            elif count == 3:
                data3 = decoded_packet
                x = 1
    while(x==1):
        # สร้าง timestamp ปัจจุบันด้วย timezone "Asia/Bangkok" และรูปแบบวันที่
        bangkok_timezone = pytz.timezone("Asia/Bangkok")
        current_datetime = datetime.now(bangkok_timezone)
        formatted_datetime = current_datetime.strftime("%d/%m/%Y -- %H:%M")

        # บันทึกข้อมูลลงใน MongoDB
        data_to_insert = {
            "_sid": data1,
            "_datetime": formatted_datetime,
            "_weight": data2,
            "_height": data3
        }
        collection.insert_one(data_to_insert)

        # ปิดการเชื่อมต่อ MongoDB
        client.close()
        x = 0
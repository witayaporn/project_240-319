const express = require("express");
const http = require("https");
const socketIo = require("socket.io");
const app = express();
const server = http.createServer(app);
const io = socketIo(server , {
  cors: {
    origin: "https://whinfo.243-family.cloud", // แทนที่ด้วยโดเมนของเว็บไคลเอนต์ของคุณ
    methods: ["GET", "POST"],
    credentials: true,
  },
});

const cors = require("cors");
require("dotenv").config({ path: "./.env" });
const port = process.env.PORT || 5000;
app.use(cors());
app.use(express.json());
app.use(require("./routes/record"));
// get driver connection
const dbo = require("./db/conn");

//server.use(cors());
//server.use(express.json());
//server.use(require("./routes/record"));

io.on("connection", async (socket) => {
  await dbo.connectToServer(function (err) {
    if (err) {
      console.error(err);
    }else {
      console.log("client connected");
    }
  });
  

  // เมื่อมีการเชื่อมต่อ WebSocket จะทำการค้นหาข้อมูลในฐานข้อมูล
  let db_connect = dbo.getDb();
  db_connect
    .collection("students")
    .find({})
    .toArray()
    .then((data) => {
      // ส่งข้อมูล JSON ผ่าน WebSocket ไปยังเว็บไซต์ของคุณ
      socket.emit("recordData", data); // ใช้ชื่อเหตุการณ์ "recordData" หรือชื่ออื่นที่คุณต้องการ
    });
    
    setInterval(() => {
      db_connect
        .collection("students")
        .find({})
        .toArray()
        .then((data) => {
          // ส่งข้อมูล JSON ผ่าน WebSocket เมื่อมีการอัปเดตในฐานข้อมูล
          socket.emit("recordData", data); // ใช้ชื่อเหตุการณ์ "recordData" หรือชื่ออื่นที่คุณต้องการ
        });
    }, 1000);

  
  
  // ทำการสื่อสารกับเว็บไคลเอนต์ผ่าน socket.emit และ socket.on ตามความต้องการ
});


/*
app.listen(port, async () => {
  // perform a database connection when server starts
  await dbo.connectToServer(function (err) {
    if (err) console.error(err);
   });
  console.log(`Server is running on port: ${port}`);
}); */

server.listen(port, async () => {
  console.log(`Server is running on port: ${port}`);
});




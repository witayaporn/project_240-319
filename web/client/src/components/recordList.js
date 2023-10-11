import React, { useEffect, useState } from "react";
import io from "socket.io-client";


export default function RecordList() {
  const [records, setRecords] = useState([]);

  // This method fetches the records from the database.
  useEffect(() => {
    async function fetchRecords() { // เปลี่ยนชื่อฟังก์ชันเป็น fetchRecords
      try {
        const socket = io("https://whinfo.243-family.cloud:5000", {
            withCredentials: true,
        });
        let response = await fetch(`http://172.16.100.9:5000/record/`);
        if (!response.ok) {
          const message = `error: ${response.statusText}`;
          window.alert(message);
          return;
        }
        socket.on("recordData", (data) => {
            // รับข้อมูลจาก WebSocket Server และทำอะไรกับข้อมูลตามที่คุณต้องการ
            console.log(data);
            setRecords(data); // ตัวอย่างการอัปเดตข้อมูล records
        });
        return () => {
            socket.disconnect();
        };
      } catch (error) {
        console.error("error:", error);
      }
    }

    fetchRecords();
  }, []); // ไม่ต้องมี records.length ใน dependencies

  // This following section will display the table with the records of individuals.
  return (
    <div className="container">
      <div className="row">
        <h1 className="mt-3">Weight-Height Information of Students</h1>
        <hr className="mb-3" /> {/* แก้ไขการใช้เครื่องหมายเท่ากับ (=) เป็นเครื่องหมายไม่เท่ากับ (-) */}
      </div>

      <table className="table table-striped" style={{ marginTop: 20 }}>
        <thead>
          <tr>
            <th style={{ paddingRight: 50 }}>ID</th>
            <th>Date-Time</th>
            <th>Weight</th>
            <th>Height</th>
          </tr>
        </thead>
        <tbody>
          {records.map(record => (
            <tr key={record._id}>
              <td>{record._sid}</td>
              <td>{record._datetime}</td>
              <td>{record._weight}</td>
              <td>{record._height}</td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
}

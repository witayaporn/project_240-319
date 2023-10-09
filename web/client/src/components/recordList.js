import React, { useEffect, useState } from "react";

export default function RecordList() {
  const [records, setRecords] = useState([]);

  // This method fetches the records from the database.
  useEffect(() => {
    async function fetchRecords() { // เปลี่ยนชื่อฟังก์ชันเป็น fetchRecords
      try {
        let response = await fetch(`http://localhost:27017/record/`);
        if (!response.ok) {
          const message = `error: ${response.statusText}`;
          window.alert(message);
          return;
        }
        const data = await response.json(); // เปลี่ยนชื่อตัวแปรเป็น data
        setRecords(data);
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
          {records.map((record) => (
            <tr key={record.id}>
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

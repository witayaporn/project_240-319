import React from "react";
import { Route, Routes } from "react-router-dom";
import RecordList from "./components/recordList";

function App() {
  return (
    <div>
      <Routes>
        <Route exact path="/" element={<RecordList />} />
      </Routes>
    </div>
  );
}

export default App;

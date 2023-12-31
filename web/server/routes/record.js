const express = require("express");
// recordRoutes is an instance of the express router.
// We use it to define our routes.
// The router will be added as a middleware and will take control of requests starting with path /record.
const recordRoutes = express.Router();
 
// This will help us connect to the database
const dbo = require("../db/conn");
 
// This help convert the id from string to ObjectId for the _id.
const ObjectId = require("mongodb").ObjectId;
 
recordRoutes.route("/record").get(async function (req, response) {
 let db_connect = dbo.getDb();
 db_connect
   .collection("students")
   .find({})
   .toArray()
   .then((data) => {
      console.log(data);
      response.json(data);
   });
});
 
// This section will help you get a single record by id
recordRoutes.route("/record/:id").get(async function (request, response) {
 let db_connect = dbo.getDb();
 let myquery = { _id: new ObjectId(request.params.id) };
 db_connect
   .collection("students")
   .find(myquery)
   .toArray()
   .then((data) => {
    console.log(data);
    response.json(data);
   })
});
 
module.exports = recordRoutes;


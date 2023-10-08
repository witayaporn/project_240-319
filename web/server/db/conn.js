console.log("devr");
const { MongoClient } = require("mongodb");
const Db = process.env.ATLAS_URI;
if (Db === undefined) { throw new Error("My ATLAS_URI is undefined");}
const client = new MongoClient(Db, {
  useNewUrlParser: true,
  useUnifiedTopology: true,
});
 
let _db;
 
module.exports = {
  connectToServer: async function (callback) {
    try {
      await client.connect();
    } catch (e) {
      console.error(e);
    }
    _db = client.db("project");

    return (_db === undefined ? false : true);
  },
  getDb: function () {
    return _db;
  },
};
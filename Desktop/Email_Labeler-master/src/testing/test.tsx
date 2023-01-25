import * as fs from "fs"
export function csvParse(csvPath: string) {
    const csv = require("csv-parser");
    const results: any[] = [];
    fs.createReadStream(csvPath)
      .pipe(csv())
      .on('data', (data: any) => results.push(data))
      .on('end', () => console.log(results))
    
}
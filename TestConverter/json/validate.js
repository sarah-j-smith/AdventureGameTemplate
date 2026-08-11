import fs from 'fs';
import Ajv from 'ajv';

const ajv = new Ajv({ allErrors: true, strict: false }); // allErrors finds all errors instead of stopping at the first

try {
  // 1. Load the files from disk
  const schema = JSON.parse(fs.readFileSync('./json_reporter_migrated.schema.json', 'utf8'));
  const data = fs.readFileSync('./DartTestOutputExample.jsonl', 'utf8');

  // 2. Compile the schema and validate the data
  const validate = ajv.compile(schema);

  let valid = true;
  
  const lines = data.split(/\s+/);
  lines.forEach((ln, ix) => {
    const lnOk = validate(ln);
    if (valid) {
      console.log(`Line ${ix} - ✅ ${ln}`);
    } else {
      console.error(`Line ${ix} - ❌ ${ln} - ${validate.errors}`);
    }
 });
} catch (error) {
  console.error('An error occurred:', error.message);
}
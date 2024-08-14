# Running Test of functionality

## Running All Tests

To execute the entire suite of test cases:

1. Navigate to the `functionality` directory:
   ```bash
   cd functionality
   ```

2. Compile the tests by running:
   ```bash
   make
   ```

3. Start the test suite with:
   ```bash
   ./run_test.sh
   ```

The `run_test.sh` script will automatically run all 18 test cases in order.

## Running a Single Test Case

To execute a specific test case:

```bash
./client test{num}.xml
```

Replace `{num}` with the test case number (e.g., `01`, `02`, ..., `18`).


-  If necessary, change the permissions with:
  ```bash
  chmod +x run_test.sh
  chmod +x client
  ```


---
# Running Test of Scalability



1. Navigate to the `scalability` directory:
   ```bash
   cd scalability
   ```

2. Compile the tests by running:
   ```bash
   make
   ```
3. Change the instances in `run_all.sh`attribute to limit he number of concurrent clients:
   ```run_all.sh
   instances={num}
   ```

3. Start the test suite with:
   ```bash
   ./run_all.sh
   ```
4. The output for each client will show in corresponding `output_{num}.txt`


The `run_all.sh` script will automatically run all 30 test cases 50 times for different core limitation(1,2,4 and 8).


-  If necessary, change the permissions with:
  ```bash
  chmod +x run_all.sh
  chmod +x run_test.sh
  chmod +x client
  ```
-  If necessary, install the bc command with:
  ```bash
  sudo apt-get install bc
  ```

---
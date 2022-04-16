import sys
import os
import subprocess
import time
import logging


if __name__ == '__main__':

    directory = sys.argv[1]

    logging.basicConfig(
        level=logging.INFO,
        filename='log.log',
        format='%(levelname)s:%(asctime)s %(msg)s'
    )

    files = os.listdir(directory)

    fulltime: int = 0
    for file in files:

        start = time.time()
        file = os.path.join(directory, file)
        r = subprocess.run(['./build/main.o', file],)
        logging.info(f"file: {file} - { 1 if r.returncode == 1 else 0}")
        end = time.time()

        fulltime += end - start

    logging.info(f'Average time {fulltime/len(files)}')
    print(f'Average time {fulltime/len(files)}')


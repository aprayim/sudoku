import argparse
from re import sub
import subprocess
import os
import random
import string
import sys
import logging

logging.basicConfig(level=logging.INFO)

def createValidBoard(binary: str, output: str) -> None:
    
    cmd = [binary, '--output_file', output, '-logtostderr']
    try:
        logging.info(f'running {" ".join(cmd)}')
        subprocess.run(cmd, check=True, capture_output=True)
    except subprocess.CalledProcessError as e:
        logging.info(e.returncode.decode("utf-8"))
        logging.info(e.stderr("utf-8"))
        sys.exit(1)

def createPuzzleFromValidBoard(binary: str, n: int, valid_board: str, output_base: str) -> None:

    cmd = [binary, '--valid_board', valid_board, '--output_base', output_base, '--n', str(n), '--logtostderr']

    try:
        logging.info(f'running {" ".join(cmd)}')
        subprocess.run(cmd, check=True, capture_output=True)
    except subprocess.CalledProcessError as e:
        logging.info(e.returncode("utf-8"))
        logging.info(e.stderr("utf-8"))
        sys.exit(1)

def do_work(outdir: str, bindir: str, number_to_create: int) -> bool:
    
    create_binary = f'{bindir}/valid_board'
    puzzle_binary = f'{bindir}/puzzle_generator'

    filebase = ''.join(random.choices(string.ascii_lowercase, k=5))
    valid_board_dir = f'{outdir}/valid_board'
    puzzle_dir = f'{outdir}/puzzle_dir'
    os.makedirs(valid_board_dir, exist_ok=True)
    os.makedirs(puzzle_dir, exist_ok=True)

    createValidBoard(create_binary, f'{valid_board_dir}/{filebase}')
    createPuzzleFromValidBoard(puzzle_binary, number_to_create, f'{valid_board_dir}/{filebase}.sudoku', f'{puzzle_dir}/{filebase}')

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="creates puzzles from a valid board")
    parser.add_argument("--outdir", required=True, type=str, help="output directory")
    parser.add_argument("--bindir", default=".", type=str, help="location of binaries")
    parser.add_argument("--number_to_create", required=True, type=int, help="number of puzzles to create")
    args = parser.parse_args()

    do_work(args.outdir, args.bindir, args.number_to_create)
#!/usr/bin/env python3
import sys
from pathlib import Path

def parse_header(header_path: Path):
    lines = header_path.read_text().splitlines()
    docs = []
    current = []
    for line in lines:
        if line.strip().startswith('/**'):
            current = [line]
        elif current:
            current.append(line)
            if line.strip().endswith('*/'):
                docs.append('\n'.join(current))
                current = []
    return docs


def convert_to_markdown(block: str) -> str:
    lines = []
    for line in block.splitlines():
        line = line.strip()
        if line.startswith('/**') or line.startswith('*/'):
            continue
        line = line.lstrip('*').strip()
        lines.append(line)
    return '\n'.join(lines).strip()


def main(header_path: str, intro_path: str, output_path: str):
    header = Path(header_path)
    intro = Path(intro_path)
    output = Path(output_path)

    sections = ["# API documentation", intro.read_text().strip(), "## Extracted comments"]
    for block in parse_header(header):
        md = convert_to_markdown(block)
        if md:
            sections.append(f"- {md}")

    output.write_text('\n\n'.join(sections) + '\n')


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: extract_docs.py <header> <intro.md> <output.md>")
        sys.exit(1)
    main(sys.argv[1], sys.argv[2], sys.argv[3])

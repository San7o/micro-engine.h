#!/bin/sh
#
# generate.sh
# ===========
#
# Generate a static website from markdown files.
#
# SPDX-License-Identifier: MIT
# Author:  Giovanni Santini
# Mail:    giovanni.santini@proton.me
# Github:  @San7o
#
# Usage
# -----
#
#   chmod +x ./generate.sh
#   ./generate.sh
#

set -e

#
# Configuration
#

# Set this according to your website's URL
BASE_URL="https://san7o.github.io/micro-website"

# Directory containing the contents (.md files) and additional files
# (favicon, robots.txt) of the website
FILES_DIR=$(pwd)

# Directory where the website will be generated
HTML_DIR="html"

# The intro file will be prepended to each .html file
HTML_INTRO="intro.html"

# The outro file will be appended to each .html files
HTML_OUTRO="outro.html"

# Specify an html index file
# INDEX_FILE="index.html"

# A scratch file that will be used by this script somehow
TMP_FILE="/tmp/generated.html"

# Website's favicon
FAVICON_FILE="favicon.ico"

# Website's robots.txt
ROBOTS_FILE="robots.txt"

# Pandoc arguments
PANDOC_FLAGS=""

# Images directory
IMG_DIR="./"

DEMO_DIR="./demo"

#
# Generation
#

if [ ! -d "$HTML_DIR" ]; then
  echo "Creating html directory"
  mkdir -p "$HTML_DIR"
fi

if [ -f "$HTML_DIR/sitemap.txt" ]; then
    rm "$HTML_DIR/sitemap.txt"
fi

if [ -f "$INDEX_FILE" ]; then
  echo "Generating $HTML_DIR/index.html"
  {
      cat "$HTML_INTRO"
      cat "$INDEX_FILE"
      cat "$HTML_OUTRO"
  } > "$HTML_DIR/index.html"
  echo "$BASE_URL/index.html" >> "$HTML_DIR/sitemap.txt"
fi

for md in "$FILES_DIR"/*.md; do
  [ -e "$md" ] || continue

  base=$(basename "$md" .md)
  out="$HTML_DIR/$base.html"

  # Get title and description
  eval "$(
awk '
  $0=="---" {start = !start; next}
  start {
    if ($1=="title:") {
      $1=""; sub(/^ /, ""); printf("title=\"%s\"\n", $0)
    }
    if ($1=="description:") {
      $1=""; sub(/^ /, ""); printf("description=\"%s\"\n", $0)
    }
  }
' $md
)"
  
  echo "Generating $out"
  pandoc "$md" -o "$TMP_FILE" $PANDOC_FLAGS

  {
      cat "$HTML_INTRO"
      cat "$TMP_FILE"
      cat "$HTML_OUTRO"
  } > "$out"

  if [ "$description" != "" ]; then
      sed -i "s/<head>/<head>\n<meta property=\"og:description\" content=\"$description\">\n<meta name=\"description\" content=\"$description\">/" "$out"
  fi
  if [ "$title" != "" ]; then
      sed -i "s/<head>/<head>\n<meta property=\"og:title\" content=\"$title\">\n<title>$title<\/title>/" "$out"
  fi

  
  echo "$BASE_URL/$base.html" >> "$HTML_DIR/sitemap.txt"
done

echo "Copying favicon.ico"
cp "$FAVICON_FILE" "$HTML_DIR/favicon.ico"

echo "Copying robots.txt"
cp "$ROBOTS_FILE" "$HTML_DIR/robots.txt"
echo -e "\nSitemap: $BASE_URL/sitemap.txt" >> "$HTML_DIR/robots.txt"

echo "Copying images"
cp "$IMG_DIR"/*.jpg "$HTML_DIR/"

echo "Copying demo dir"
mkdir -p "$HTML_DIR/demo"
cp "$DEMO_DIR"/* "$HTML_DIR/demo"

echo "Done."

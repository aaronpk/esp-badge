<?php
// Create a 128x250 pixel grayscale png image
$im = imagecreatefrompng('image.png');

$bits = '';
$pixelcount = 0;

for($y = 0; $y < imagesy($im); $y++) {
  for($x = 0; $x < imagesx($im); $x++) {
    $rgb = imagecolorat($im, $x, $y);

    $color = $rgb & 0xFF;

    if($color == 1) {
      $bits .= '1';
    } else {
      $bits .= '0';
    }
    $pixelcount++;

    if($pixelcount % 8 == 0) {
      $bytes .= encode_bits($bits);
      $bits = '';
    }
  }
}

if($bits != '') {
  $bytes .= encode_bits($bits);
}

$size = strlen($bytes);
header('Content-type: text/plain');
header("Content-length: $size");
echo $bytes;

function encode_bits($bits) {
  return pack('H*', str_pad(base_convert($bits, 2, 16), 2, "0", STR_PAD_LEFT));
}

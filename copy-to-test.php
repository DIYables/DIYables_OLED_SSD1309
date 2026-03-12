<?php

// Get the current folder name dynamically
$currentFolderName = basename(__DIR__);
echo "Copying $currentFolderName library to test folder...\n\n";

// Define source and destination paths
$sourceDir = __DIR__;
$destDir = 'C:\Users\KHANH\Documents\Arduino\libraries\\' . $currentFolderName;

// Create destination directory if it doesn't exist
if (!file_exists($destDir)) {
    mkdir($destDir, 0777, true);
    echo "Created destination directory: $destDir\n";
}

// Define simple key-value replacements for sensitive information
// Add your actual sensitive values as keys and their replacements as values
$replacements = [
    
];

// Function to recursively copy directory
function copyDirectory($src, $dst, $replacements) {
    if (!file_exists($dst)) {
        mkdir($dst, 0777, true);
    }
    
    $dir = opendir($src);
    while (($file = readdir($dir)) !== false) {
        if ($file != '.' && $file != '..') {
            $srcPath = $src . '/' . $file;
            $dstPath = $dst . '/' . $file;
            
            if (is_dir($srcPath)) {
                copyDirectory($srcPath, $dstPath, $replacements);
            } else {
                copyFileWithReplacements($srcPath, $dstPath, $replacements);
            }
        }
    }
    closedir($dir);
}

// Function to copy file and apply replacements
function copyFileWithReplacements($srcFile, $dstFile, $replacements) {
    // Read the source file
    $content = file_get_contents($srcFile);
    
    // Check if it's a text file that should be processed
    $extension = pathinfo($srcFile, PATHINFO_EXTENSION);
    $textExtensions = ['cpp', 'h', 'ino', 'txt', 'md', 'properties', 'json', 'xml'];
    
    if (in_array(strtolower($extension), $textExtensions)) {
        // Apply simple string replacements
        foreach ($replacements as $search => $replace) {
            $content = str_replace($search, $replace, $content);
        }
    }
    
    // Write to destination
    file_put_contents($dstFile, $content);
}

// Copy folders with replacements
echo "Copying examples folder...\n";
copyDirectory($sourceDir . '/examples', $destDir . '/examples', $replacements);

echo "Copying src folder...\n";
copyDirectory($sourceDir . '/src', $destDir . '/src', $replacements);

// Copy individual files with replacements
echo "Copying keywords.txt...\n";
copyFileWithReplacements($sourceDir . '/keywords.txt', $destDir . '/keywords.txt', $replacements);

echo "Copying library.properties...\n";
copyFileWithReplacements($sourceDir . '/library.properties', $destDir . '/library.properties', $replacements);

echo "Copying license.txt...\n";
copyFileWithReplacements($sourceDir . '/license.txt', $destDir . '/license.txt', $replacements);

echo "Copying README.md...\n";
copyFileWithReplacements($sourceDir . '/README.md', $destDir . '/README.md', $replacements);

echo "\n=============================================\n";
echo "Copy completed successfully!\n";
echo "Destination: $destDir\n";
echo "All sensitive information has been replaced.\n";
echo "=============================================\n";

?>

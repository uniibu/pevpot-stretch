var binding;
try {
    binding = new require('./build/Debug/stretch');
} catch(e) {
    binding = new require('./build/Release/stretch');
}

module.exports = binding;

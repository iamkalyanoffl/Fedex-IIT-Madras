var path = require('path');

// Constants and fixtures for nodejs tests on our Monaco dataset.

// Somewhere in Monaco
// http://www.openstreetmap.org/#map=18/43.73185/7.41772
exports.three_test_coordinates = [[7.41337, 43.72956],
                                  [7.41546, 43.73077],
                                  [7.41862, 43.73216]];

exports.two_test_coordinates = exports.three_test_coordinates.slice(0, 2)

exports.test_tile = {'at': [17059, 11948, 15], 'size': 114000};


// Test files generated by the routing engine; check test/data
if (process.env.OSRM_DATA_PATH !== undefined) {
    exports.data_path = path.join(path.resolve(process.env.OSRM_DATA_PATH), "ch/monaco.osrm");
    exports.mld_data_path = path.join(path.resolve(process.env.OSRM_DATA_PATH), "mld/monaco.osrm");
    exports.corech_data_path = path.join(path.resolve(process.env.OSRM_DATA_PATH), "corech/monaco.osrm");
    console.log('Setting custom data path to ' + exports.data_path);
} else {
    exports.data_path = path.resolve(path.join(__dirname, "../data/ch/monaco.osrm"));
    exports.mld_data_path = path.resolve(path.join(__dirname, "../data/mld/monaco.osrm"));
    exports.corech_data_path = path.resolve(path.join(__dirname, "../data/corech/monaco.osrm"));
}

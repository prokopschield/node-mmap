type mmap_t = (filename: string, readonly: boolean) => Buffer;
const mmap = require('../build/Release/mmap.node') as mmap_t;

Object.defineProperties(mmap, {
	default: {
		enumerable: false,
		value: mmap,
	},
	mmap: {
		enumerable: false,
		value: mmap,
	},
});

module.exports = mmap;

export default mmap;
export { mmap };

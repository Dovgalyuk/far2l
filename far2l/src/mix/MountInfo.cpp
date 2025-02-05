#include "headers.hpp"

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <fcntl.h>
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__CYGWIN__)
# include <errno.h>
# include <sys/mount.h>
#else
# include <sys/statfs.h>
# include <linux/fs.h>
#endif
#include <fstream>
#include "MountInfo.h"
#include <ScopeHelpers.h>
#include <os_call.hpp>

static struct FSMagic {
	const char *name;
	unsigned int magic;
} s_fs_magics[] = {
{"ADFS",	0xadf5},
{"AFFS",	0xadff},
{"AFS",                0x5346414F},
{"AUTOFS",	0x0187},
{"CODA",	0x73757245},
{"CRAMFS",		0x28cd3d45},	/* some random number */
{"CRAMFS",	0x453dcd28},	/* magic number with the wrong endianess */
{"DEBUGFS",          0x64626720},
{"SECURITYFS",	0x73636673},
{"SELINUX",		0xf97cff8c},
{"SMACK",		0x43415d53},	/* "SMAC" */
{"RAMFS",		0x858458f6},	/* some random number */
{"TMPFS",		0x01021994},
{"HUGETLBFS", 	0x958458f6},	/* some random number */
{"SQUASHFS",		0x73717368},
{"ECRYPTFS",	0xf15f},
{"EFS",		0x414A53},
{"EXT2",	0xEF53},
{"EXT3",	0xEF53},
{"XENFS",	0xabba1974},
{"EXT4",	0xEF53},
{"BTRFS",	0x9123683E},
{"NILFS",	0x3434},
{"F2FS",	0xF2F52010},
{"HPFS",	0xf995e849},
{"ISOFS",	0x9660},
{"JFFS2",	0x72b6},
{"PSTOREFS",		0x6165676C},
{"EFIVARFS",		0xde5e81e4},
{"HOSTFS",	0x00c0ffee},

{"MINIX",	0x137F},		/* minix v1 fs, 14 char names */
{"MINIX",	0x138F},		/* minix v1 fs, 30 char names */
{"MINIX2",	0x2468},		/* minix v2 fs, 14 char names */
{"MINIX2",	0x2478},		/* minix v2 fs, 30 char names */
{"MINIX3",	0x4d5a},		/* minix v3 fs, 60 char names */

{"MSDOS",	0x4d44},		/* MD */
{"NCP",		0x564c},		/* Guess, what 0x564c is :-) */
{"NFS",		0x6969},
{"OPENPROM",	0x9fa1},
{"QNX4",	0x002f},		/* qnx4 fs detection */
{"QNX6",	0x68191122},	/* qnx6 fs detection */

{"REISERFS",	0x52654973},	/* used by gcc */
					/* used by file system utilities that
	                                   look at the superblock, etc.  */
{"SMB",		0x517B},
{"CGROUP",	0x27e0eb},


{"STACK_END",		0x57AC6E9D},

{"TRACEFS",          0x74726163},

{"V9FS",		0x01021997},

{"BDEVFS",            0x62646576},
{"BINFMTFS",          0x42494e4d},
{"DEVPTS",	0x1cd1},
{"FUTEXFS",	0xBAD1DEA},
{"PIPEFS",            0x50495045},
{"PROC",	0x9fa0},
{"SOCKFS",		0x534F434B},
{"SYSFS",		0x62656572},
{"USBDEVICE",	0x9fa2},
{"MTD_INODE_FS",      0x11307854},
{"ANON_INODE_FS",	0x09041934},
{"BTRFS_TEST",	0x73727279},
{"NSFS",		0x6e736673},
{"BPF_FS",		0xcafe4a11}};


/////////////////////////////////////////////////////////////////////////////////////////////

MountInfo::MountInfo()
{
#ifdef __linux__
	// force-enable multi-threaded disk access: echo e > ~/.config/far2l/mtfs
	// force-disable multi-threaded disk access: echo d > ~/.config/far2l/mtfs
	FDScope fd(open(InMyConfig("mtfs").c_str(), O_RDONLY));
	if (fd.Valid()) {
		if (os_call_ssize(read, (int)fd, (void *)&_mtfs, sizeof(_mtfs)) == 0) {
			_mtfs = 'e';
		}
		fprintf(stderr, "%s: _mtfs='%c'\n", __FUNCTION__, _mtfs);
	}

	std::ifstream is("/proc/mounts");
	if (is.is_open()) {
		std::string line, sys_path;
		std::vector<std::string> parts;
		while (std::getline(is, line)) {
			parts.clear();
			StrExplode(parts, line, " \t");
			if (parts.size() > 1 && StrStartsFrom(parts[1], "/")) {
				bool multi_thread_friendly;
				if (StrStartsFrom(parts[0], "/dev/")) {
					sys_path = "/sys/block/";
					sys_path+= parts[0].substr(5);
					// strip device suffix, sda1 -> sda, mmcblk0p1 -> mmcblk0
					struct stat s;
					while (sys_path.size() > 12 && stat(sys_path.c_str(), &s) == -1) {
						sys_path.resize(sys_path.size() - 1);
					}
					sys_path+= "/queue/rotational";
					char c = '?';
					FDScope fd(open(sys_path.c_str(), O_RDONLY));
					if (fd.Valid()) {
						os_call_ssize(read, (int)fd, (void *)&c, sizeof(c));
					} else {
						fprintf(stderr, "%s: can't read '%s'\n", __FUNCTION__, sys_path.c_str());
					}
					multi_thread_friendly = (c == '0');
				} else {
					multi_thread_friendly = (parts[0] == "tmpfs" || parts[0] == "proc");
				}
				if (parts.size() == 1) {
					parts.emplace_back();
				}
				_mountpoints.emplace_back(Mountpoint{parts[1], parts[2], multi_thread_friendly});
				fprintf(stderr, "%s: mtf=%d fs='%s' for '%s' at '%s'\n", __FUNCTION__,
					multi_thread_friendly, parts[2].c_str(), parts[0].c_str(), parts[1].c_str());
			}
		}
	}
	if (_mountpoints.empty()) {
		fprintf(stderr, "%s: failed to parse /proc/mounts\n", __FUNCTION__);
	}
#endif

	// TODO: BSD, MacOS
}


std::string MountInfo::GetFileSystem(const std::string &path) const
{
	std::string out;
	size_t longest_match = 0;
	for (const auto &it : _mountpoints) {
		if (it.path.size() > longest_match && StrStartsFrom(path, it.path.c_str())) {
			longest_match = it.path.size();
			out = it.filesystem;
		}
	}

	if (out.empty()) {
		struct statfs sfs{};
#if !defined(__FreeBSD__) && !defined(__CYGWIN__)
		if (sdc_statfs(path.c_str(), &sfs) == 0) {
#else
		if (statfs(path.c_str(), &sfs) == 0) {
#endif
#ifdef __APPLE__
		out = sfs.f_fstypename;
		if (out.empty())
#endif
			for (size_t i = 0; i < ARRAYSIZE(s_fs_magics); ++i) {
				if (sfs.f_type == s_fs_magics[i].magic) {
					out = s_fs_magics[i].name;
					break;
				}
			}
		}
	}

	return out;
}

bool MountInfo::IsMultiThreadFriendly(const std::string &path) const
{
	if (_mtfs != 0) {
		return (_mtfs == 'e' || _mtfs == 'E');
	}

	bool out = true;
	size_t longest_match = 0;
	for (const auto &it : _mountpoints) {
		if (it.path.size() > longest_match && StrStartsFrom(path, it.path.c_str())) {
			longest_match = it.path.size();
			out = it.multi_thread_friendly;
		}
	}
	return out;
}

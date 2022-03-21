#include "inode.h"
#include "storage.h"
#include "file.h"
#include <bitset>
#include <cstring>

INode root_inode;
const INode &ROOT_INODE = root_inode;

std::bitset<INODE_MAX_NUMBER + 1> used_inode;

inline bool isValidINodeNumber(int num) {
	return 1 <= num <= INODE_MAX_NUMBER;
}

inline bool getINodeAddress(int num) {
	return INODE_MEMORY_START + (num - 1) * INODE_SIZE;
}

int getFreeINodeNumber() {
	for (int i = 1; i <= INODE_MAX_NUMBER; ++i)
		if (!used_inode[i])
			return i;
	return 0;
}

INode::INode() {
	memset(this, 0, sizeof (*this));
}

INode createINode() {
	if (used_inode.all())
		throw INodeError({0, "All INodes are in use!", "createINode"});
	INode res_inode;
	res_inode.num = getFreeINodeNumber();
	res_inode.create_time = time(NULL);
	putBlock(getINodeAddress(res_inode.num), &res_inode, INODE_SIZE);
	used_inode.set(res_inode.num);
	return res_inode;
}

INode selectINode(int num) {
	if (!isValidINodeNumber(num))
		throw INodeError({num, "The INode number is invalid!", "selectINode"});
	if (!used_inode[num])
		throw INodeError({num, "The INode number is not in use!", "selectINode"});
	INode res_inode;
	getBlock(getINodeAddress(num), &res_inode, INODE_SIZE);
	return res_inode;
}

void updateINode(const INode &inode) {
	if (!isValidINodeNumber(inode.num))
		throw INodeError({inode.num, "The INode number is invalid!", "updateINode"});
	if (!used_inode[inode.num])
		throw INodeError({inode.num, "The INode number is not in use!", "updateINode"});
	putBlock(getINodeAddress(inode.num), &inode, INODE_SIZE);
}

void deleteINode(int num) {
	if (!isValidINodeNumber(num))
		throw INodeError({num, "The INode number is invalid!", "deleteINode"});
	putBlock(getINodeAddress(num), &BLANK_INODE, INODE_SIZE);
	used_inode.reset(num);
}

void inodeInitializer() {
	used_inode.reset();
	used_inode.set(0); // 0 is reserved
	for (int i = INODE_MEMORY_START; i < INODE_MEMORY_END; i += INODE_SIZE) {
		INode tmp;
		getBlock(i, &tmp, INODE_SIZE);
		if (tmp.num != 0)
			used_inode.set(tmp.num);
	}
	if (used_inode.none()) {
		root_inode = createINode();
		initDir(root_inode);
	} else
		root_inode = selectINode(1); // the Inode number for / is always 1
}

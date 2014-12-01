/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef ILLUSIONS_RESOURCESYSTEM_H
#define ILLUSIONS_RESOURCESYSTEM_H

#include "common/array.h"
#include "common/file.h"
#include "common/hashmap.h"
#include "common/memstream.h"
#include "common/str.h"
#include "common/substream.h"
#include "common/system.h"

namespace Illusions {

#define ResourceTypeId(x) ((x) & 0xFFFF0000)

class BaseResourceLoader;
class IllusionsEngine;
struct Resource;

class ResourceInstance {
public:
	virtual void load(Resource *resource);
	virtual void unload();
	virtual ~ResourceInstance();
};

struct Resource {
	bool _loaded;
	uint32 _resId;
	uint32 _tag;
	uint32 _threadId;
	byte *_data;
	uint32 _dataSize;
	int _gameId;
	Common::String _filename;
	ResourceInstance *_instance;
	Resource() : _loaded(false), _resId(0), _tag(0), _threadId(0), _data(0), _dataSize(0), _instance(0) {
	}
	~Resource() {
		if (_instance)
			_instance->unload();
		delete _instance;
		unloadData();
	}
	void loadData();
	void unloadData();
};

enum {
	kRlfLoadFile,
	kRlfFreeDataAfterLoad
};

class BaseResourceLoader {
public:
	virtual ~BaseResourceLoader() {}
	virtual void load(Resource *resource) = 0;
	virtual void unload(Resource *resource) = 0;
	virtual void buildFilename(Resource *resource) = 0;
	virtual bool isFlag(int flag) = 0;
};

// TODO Possibly split resource loaders from the system?

class ResourceSystem {
public:
	ResourceSystem(IllusionsEngine *vm);
	~ResourceSystem();

	void addResourceLoader(uint32 resTypeId, BaseResourceLoader *resourceLoader);
	
	// TODO Handle threadId in caller as well as pausing of timer
	void loadResource(uint32 resId, uint32 tag, uint32 threadId);
	void unloadResourceById(uint32 resId);
	void unloadResourcesByTag(uint32 tag);
	void unloadSceneResources(uint32 sceneId1, uint32 sceneId2);
	
protected:
	typedef Common::HashMap<uint32, BaseResourceLoader*> ResourceLoadersMap;
	typedef ResourceLoadersMap::iterator ResourceLoadersMapIterator;
	IllusionsEngine *_vm;
	ResourceLoadersMap _resourceLoaders;
	BaseResourceLoader *getResourceLoader(uint32 resId);

	typedef Common::Array<Resource*> ResourcesArray;
	typedef ResourcesArray::iterator ResourcesArrayIterator;
	ResourcesArray _resources;

	struct ResourceEqualById : public Common::UnaryFunction<const Resource*, bool> {
		uint32 _resId;
		ResourceEqualById(uint32 resId) : _resId(resId) {}
		bool operator()(const Resource *resource) const {
			return resource->_resId == _resId;
		}
	};

	struct ResourceEqualByValue : public Common::UnaryFunction<const Resource*, bool> {
		const Resource *_resource;
		ResourceEqualByValue(const Resource *resource) : _resource(resource) {}
		bool operator()(const Resource *resource) const {
			return resource == _resource;
		}
	};

	struct ResourceEqualByTag : public Common::UnaryFunction<const Resource*, bool> {
		uint32 _tag;
		ResourceEqualByTag(uint32 tag) : _tag(tag) {}
		bool operator()(const Resource *resource) const {
			return resource->_tag == _tag;
		}
	};

	struct ResourceNotEqualByScenes : public Common::UnaryFunction<const Resource*, bool> {
		uint32 _sceneId1, _sceneId2;
		ResourceNotEqualByScenes(uint32 sceneId1, uint32 sceneId2) : _sceneId1(sceneId1), _sceneId2(sceneId2) {}
		bool operator()(const Resource *resource) const {
			return resource->_tag != _sceneId1 && resource->_tag != _sceneId2;
		}
	};

	Resource *getResource(uint32 resId);
	void unloadResource(Resource *resource);
	
};

} // End of namespace Illusions

#endif // ILLUSIONS_RESOURCESYSTEM_H

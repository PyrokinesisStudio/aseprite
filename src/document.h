/* ASE - Allegro Sprite Editor
 * Copyright (C) 2001-2011  David Capello
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef DOCUMENT_H_INCLUDED
#define DOCUMENT_H_INCLUDED

#include "base/disable_copying.h"
#include "base/shared_ptr.h"
#include "document_id.h"

#include <string>

class Cel;
class FormatOptions;
class Image;
class Mask;
class Mutex;
class Sprite;
class UndoHistory;
struct _BoundSeg;

struct PreferredEditorSettings
{
  int scroll_x;
  int scroll_y;
  int zoom;
  bool virgin;
};

enum DuplicateType
{
  DuplicateExactCopy,
  DuplicateWithFlattenLayers,
};

// An application document. It is the class used to contain one file
// opened and being edited by the user (a sprite).
class Document
{
public:

  // Creates a document with one sprite, with one transparent layer,
  // and one frame.
  static Document* createBasicDocument(int imgtype, int width, int height, int ncolors);

  Document(Sprite* sprite);
  ~Document();

  //////////////////////////////////////////////////////////////////////
  // Main properties
  
  DocumentId getId() const { return m_id; }
  void setId(DocumentId id) { m_id = id; }

  const Sprite* getSprite() const { return m_sprite; }
  const UndoHistory* getUndoHistory() const { return m_undoHistory; }

  Sprite* getSprite() { return m_sprite; }
  UndoHistory* getUndoHistory() { return m_undoHistory; }

  //////////////////////////////////////////////////////////////////////
  // File related properties

  const char* getFilename() const;
  void setFilename(const char* filename);

  bool isModified() const;
  bool isAssociatedToFile() const;
  void markAsSaved();

  //////////////////////////////////////////////////////////////////////
  // Loaded options from file

  void setFormatOptions(const SharedPtr<FormatOptions>& format_options);

  //////////////////////////////////////////////////////////////////////
  // Preferred editor settings

  PreferredEditorSettings getPreferredEditorSettings() const;
  void setPreferredEditorSettings(const PreferredEditorSettings& settings);

  //////////////////////////////////////////////////////////////////////
  // Boundaries

  int getBoundariesSegmentsCount() const;
  const _BoundSeg* getBoundariesSegments() const;

  void generateMaskBoundaries(Mask* mask = NULL);

  //////////////////////////////////////////////////////////////////////
  // Extra Cel (it is used to draw pen preview, pixels in movement, etc.)

  void prepareExtraCel(int x, int y, int w, int h, int opacity);
  void destroyExtraCel();
  Cel* getExtraCel() const;
  Image* getExtraCelImage() const;

  //////////////////////////////////////////////////////////////////////
  // Clonning

  Document* duplicate(DuplicateType type) const;

  //////////////////////////////////////////////////////////////////////
  // Multi-threading ("sprite wrappers" use this)

  // Lock the sprite to read or write it, returning true if the sprite
  // can be accessed in the desired mode.
  bool lock(bool write);

  // If you've locked the sprite to read, using this method you can
  // raise your access level to write it.
  bool lockToWrite();

  // If you've locked the sprite to write, using this method you can
  // your access level to only read it.
  void unlockToRead();

  void unlock();

private:
  // Unique identifier for this document (it is assigned by Documents class).
  DocumentId m_id;

  Sprite* m_sprite;
  UndoHistory* m_undoHistory;

  // Document's file name (from where it was loaded, where it is saved).
  std::string m_filename;

  // True if this sprite is associated to a file in the file-system.
  bool m_associated_to_file;

  // Selected mask region boundaries
  struct {
    int nseg;
    _BoundSeg* seg;
  } m_bound;

  // Mutex to modify the 'locked' flag.
  Mutex* m_mutex;

  // True if some thread is writing the sprite.
  bool m_write_lock;

  // Greater than zero when one or more threads are reading the sprite.
  int m_read_locks;

  // Data to save the file in the same format that it was loaded
  SharedPtr<FormatOptions> m_format_options;

  // Preferred options in the editor.
  PreferredEditorSettings m_preferred;

  // Extra cel used to draw extra stuff (e.g. editor's pen preview, pixels in movement, etc.)
  Cel* m_extraCel;

  // Image of the extra cel.
  Image* m_extraImage;

  DISABLE_COPYING(Document);
};

#endif
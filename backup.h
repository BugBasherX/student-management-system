#ifndef BACKUP_H
#define BACKUP_H

void createBackup (void);   /* snapshot students.dat → backups/ */
void restoreBackup(void);   /* list backups, pick one, restore  */

#endif

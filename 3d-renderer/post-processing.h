#ifndef POST_PROCESSING_H
#define POST_PROCESSING_H

enum DOF_Mode           { DOF_OFF, NEAR, FAR };


void apply_post_processing (int mode);
void depth_of_field (int dof_mode);

#endif

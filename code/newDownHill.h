#include <stdlib.h>
#include "Macros.h"




float specialRand()
{
  return ( (rand()%2) ? ((float)rand()/(float)(RAND_MAX)) : (-1.0*(float)rand()/(float)(RAND_MAX)) ) ;
}

void CopyArray(float B[],float A[],int n,float* placeholder, float* x,int m)
{
    for(int i = 0; i < n; i++)
    {
        A[i] = B[i];
        for(int j=0;j<m;j++)
        {
          x[(i*m)+j] = placeholder[(i*m)+j];
        }
    }
}

//  Left run is A[iLeft :iRight-1].
// Right run is A[iRight:iEnd-1  ].
void BottomUpMerge(float A[],int iLeft,int iRight,int iEnd,float B[],float* placeholder, float* x,int m)
{
    int i = iLeft;
    int j = iRight;
    // While there are elements in the left or right runs...
    for (int k = iLeft; k < iEnd; k++)
    {
        // If left run head exists and is <= existing right run head.
        if (i < iRight && (j >= iEnd || A[i] <= A[j]))
        {
            B[k] = A[i];
            for(int w=0;w<m;w++)
            {
              placeholder[(k*m)+w] = x[(i*m)+w] ;
            }
            i = i + 1;
        }
        else
        {
            B[k] = A[j];
            for(int w=0;w<m;w++)
            {
              placeholder[(k*m)+w] = x[(j*m)+w] ;
            }
            j = j + 1;
        }
    }
}




// array A[] has the items to sort; array B[] is a work array
void BottomUpMergeSort(float A[],int n, float* x,int m)
{
    float B[n];
    float placeholder[n*m];
    // Each 1-element run in A is already "sorted".
    // Make successively longer sorted runs of length 2, 4, 8, 16... until whole array is sorted.
    for (int width = 1; width < n; width = 2 * width)
    {
        // Array A is full of runs of length width.
        for (int i = 0; i < n; i = i + 2 * width)
        {
            // Merge two runs: A[i:i+width-1] and A[i+width:i+2*width-1] to B[]
            // or copy A[i:n-1] to B[] ( if(i+width >= n) )
            BottomUpMerge(A, i, min(i+width, n), min(i+2*width, n), B, placeholder, x, m);
        }
        // Now work array B is full of runs of length 2*width.
        // Copy array B to array A for next iteration.
        // A more efficient implementation would swap the roles of A and B.
        CopyArray(B, A, n, placeholder, x, m);
        // Now array A is full of runs of length 2*width.
    }
}


double specialrender( float* vec,
                      float* w,
                      GLuint myFrameBuffer,
                      Shader & simpleShader,
                      Model & ourModel,
                      glm::mat4 & M,
                      glm::mat4 & V,
                      glm::mat4 & P,
                      double favg,
                      double cff,
                      Mat & imageGray,
                      int width,
                      int height,
                      glm::mat4 T,
                      glm::mat4& rotMat,
                      glm::mat4 S,
                      int n_dim,
                      float ORIGINALSCALEx,
                      float ORIGINALSCALEy
                    )
{
  std::vector< unsigned char > frameImage;
  calculateZ(vec,rotMat);

  reshape(width,height,P,vec[6]);

  Mat temp;
  if(n_dim==10)
  {
    int tx=round(w[8]*(width/10));
    int ty=round(w[9]*(height/10));
    // int tx=w[8];
    // int ty=w[9];
    Mat translation_matrix = (Mat_<float>(2,3) << 1, 0, tx,
                                                  0, 1, ty);
    warpAffine(imageGray,temp, translation_matrix,
              Size(width,height),INTER_LINEAR, BORDER_CONSTANT);

    float SCALEFACTOR=w[7];
    float SCALEFACTORx=(ORIGINALSCALEx/(P[0][0]))*SCALEFACTOR;
    float SCALEFACTORy=(ORIGINALSCALEy/(P[1][1]))*SCALEFACTOR;
    S = glm::mat4(1);
    S = glm::scale(S, glm::vec3(SCALEFACTORx,SCALEFACTORy,(SCALEFACTORx+SCALEFACTORy)/2 ));//Scale
  }
  else
  {
    temp=imageGray;
  }

  M=T*rotMat*S;
  render(
          myFrameBuffer,
          simpleShader,
          ourModel,
          M,
          V,
          P,
          width,
          height,
          frameImage
        );

  double distance=Loss(temp, frameImage, favg,cff);
  return distance;
}


void downhill(float* fw,
              float* w,
              GLuint myFrameBuffer,
              Shader & simpleShader,
              Model & ourModel,
              glm::mat4 & M,
              glm::mat4 & V,
              glm::mat4 & P,
              double favg,
              double cff,
              Mat & imageGray,
              int width,
              int height,
              glm::mat4 T,
              glm::mat4& rotMat,
              glm::mat4 S,
              int n_dim,
              int n_dimsum,
              float ORIGINALSCALEx,
              float ORIGINALSCALEy
            )
{
  //Init Variables

  // float alfa=1.0;
  // float beta=1.0;
  // float gamma=0.5;
  float alpha=2.0;
  float beta=2.0;
  float gamma=0.5;
  float sigma=0.5;
  // float sigma=0.01;
  double distance=1.0;
  double distance_reflec=1.0;
  double distance_expan=1.0;
  double distance_contrac=1.0;
  double distance_shrink=1.0;
  float last_distance=1.0;
  float var_distance=1.0;
  float reflect[n_dim];
  float tempVec[n_dim];
  float expanded[n_dim];
  float contracted[n_dim];
  float shrink[n_dim];
  float centroid[n_dim];
  for(int i=0;(i<MAX_ITER)&&(distance>MIN_ERROR)&&(fabs(var_distance)>MIN_VAR);i++)//&&(distance>MIN_ERROR)&&(fabs(var_distance)>MIN_ERROR)
  {
    //Calculate centroid

    for(int iteri=0;iteri<n_dim;iteri++)
    {
      float sum=0;
      for (int iterj = 0; iterj < n_dim; iterj++)
      {
        sum+=w[(iterj*n_dim)+iteri];
      }
      centroid[iteri]=sum/n_dim;
    }

    //Reflection

    for(int iteri=0;iteri<n_dim;iteri++)
    {
      // reflect[iteri]= centroid[iteri] + alfa*(centroid[iteri]-w[(n_dim*n_dim)+iteri]) ;
      reflect[iteri]= alpha*centroid[iteri] - w[(n_dim*n_dim)+iteri] ;
    }

    distance=specialrender(reflect,
                          w,
                          myFrameBuffer,
                          simpleShader,
                          ourModel,
                          M,
                          V,
                          P,
                          favg,
                          cff,
                          imageGray,
                          width,
                          height,
                          T,
                          rotMat,
                          S,
                          n_dim,
                          ORIGINALSCALEx,
                          ORIGINALSCALEy
                          );

    // calculateZ(reflect,rotMat);
    //
    // reshape(width,height,P,reflect[6]);
    //
    // M=T*rotMat*S;
    // render(
    //         myFrameBuffer,
    //         simpleShader,
    //         ourModel,
    //         M,
    //         V,
    //         P,
    //         width,
    //         height,
    //         frameImage
    //       );
    //
    // // ApplyMask(mask,frameImage);
    // distance=Loss(imageGray, frameImage, favg,cff);
    var_distance=last_distance-distance;
    last_distance=distance;
    distance_reflec=distance;

    cout << "\ndownhill calcule reflect: distance:" <<distance << " var_distance: " << var_distance << endl;

    //Check Reflect
    if((distance_reflec>fw[0])&&(distance_reflec<fw[n_dim]))//bigger than best and smaller than the second worst
    {
      insertionFancy(fw,distance_reflec,reflect,w,n_dim);
      continue;
    }

    //Expansion
    if(distance_reflec<fw[0])//f(xr)<f(x1)
    {
      //Do expansion
      //Calculate expanded

      for(int iteri=0;iteri<n_dim;iteri++)
      {
        expanded[iteri]= centroid[iteri] + beta*(reflect[iteri]-centroid[iteri] ) ;
        // expanded[iteri]= centroid[iteri] + beta*(centroid[iteri] - ) ;
      }

      distance=specialrender(expanded,
                            w,
                            myFrameBuffer,
                            simpleShader,
                            ourModel,
                            M,
                            V,
                            P,
                            favg,
                            cff,
                            imageGray,
                            width,
                            height,
                            T,
                            rotMat,
                            S,
                            n_dim,
                            ORIGINALSCALEx,
                            ORIGINALSCALEy
                            );
      // calculateZ(expanded,rotMat);
      //
      // reshape(width,height,P,expanded[6]);
      // M=T*rotMat*S;
      // render(
      //         myFrameBuffer,
      //         simpleShader,
      //         ourModel,
      //         M,
      //         V,
      //         P,
      //         width,
      //         height,
      //         frameImage
      //       );
      //
      // // ApplyMask(mask,frameImage);
      // distance=Loss(imageGray, frameImage, favg,cff);
      var_distance=last_distance-distance;
      last_distance=distance;
      distance_expan=distance;

      cout << "\ndownhill calcule expanded: distance:" <<distance << " var_distance: " << var_distance << endl;

      if(distance_expan<distance_reflec)
      {
        insertionFancy(fw,distance_expan,expanded,w,n_dim);
        continue;
      }
      else
      {
        insertionFancy(fw,distance_reflec,reflect,w,n_dim);
        continue;
      }
    }
    if(distance_reflec<fw[n_dimsum])//f(xr)>f(xn)
    {
      insertionFancy(fw,distance_reflec,reflect,w,n_dim);
      for(int iteri=0;iteri<n_dim;iteri++)
      {
        float sum=0;
        for (int iterj = 0; iterj < n_dim; iterj++)
        {
          sum+=w[(iterj*n_dim)+iteri];
        }
        centroid[iteri]=sum/n_dim;
      }
    }

    //Contraction
    // if(distance_reflec<fw[n_dimsum])//f(xr)>f(xn)
    // {
    //Do Contraction

    for(int iteri=0;iteri<n_dim;iteri++)
    {
      contracted[iteri]= centroid[iteri] + gamma*( w[(n_dim*n_dim)+iteri]-centroid[iteri]  ) ;
    }


    distance=specialrender(contracted,
                          w,
                          myFrameBuffer,
                          simpleShader,
                          ourModel,
                          M,
                          V,
                          P,
                          favg,
                          cff,
                          imageGray,
                          width,
                          height,
                          T,
                          rotMat,
                          S,
                          n_dim,
                          ORIGINALSCALEx,
                          ORIGINALSCALEy
                          );

    // calculateZ(contracted,rotMat);
    //
    // reshape(width,height,P,contracted[6]);
    // M=T*rotMat*S;
    // render(
    //         myFrameBuffer,
    //         simpleShader,
    //         ourModel,
    //         M,
    //         V,
    //         P,
    //         width,
    //         height,
    //         frameImage
    //       );
    // // ApplyMask(mask,frameImage);
    // distance=Loss(imageGray, frameImage, favg,cff);
    var_distance=last_distance-distance;
    last_distance=distance;
    distance_contrac=distance;

    cout << "\ndownhill calcule contract: distance:" <<distance << " var_distance: " << var_distance << endl;

    if(distance_contrac<fw[n_dim])
    {
      insertionFancy(fw,distance_contrac,contracted,w,n_dim);
      continue;
    }
    else
    {
      //Shrink
      for(int j=1;j<n_dimsum;j++)
      {
        for(int iteri=0;iteri<n_dim;iteri++)
        {
          shrink[iteri]= w[iteri] + sigma*( w[(j*n_dim)+iteri]-w[iteri]  ) ;
        }

        distance=specialrender(shrink,
                              w,
                              myFrameBuffer,
                              simpleShader,
                              ourModel,
                              M,
                              V,
                              P,
                              favg,
                              cff,
                              imageGray,
                              width,
                              height,
                              T,
                              rotMat,
                              S,
                              n_dim,
                              ORIGINALSCALEx,
                              ORIGINALSCALEy
                              );
        // calculateZ(shrink,rotMat);
        //
        // reshape(width,height,P,shrink[6]);
        // M=T*rotMat*S;
        // render(
        //         myFrameBuffer,
        //         simpleShader,
        //         ourModel,
        //         M,
        //         V,
        //         P,
        //         width,
        //         height,
        //         frameImage
        //       );
        // // ApplyMask(mask,frameImage);
        // distance=Loss(imageGray, frameImage, favg,cff);
        var_distance=last_distance-distance;
        last_distance=distance;
        distance_shrink=distance;

        cout << "\ndownhill calcule shrink: distance:" <<distance << " var_distance: " << var_distance << endl;

        for(int iteri=0;iteri<n_dim;iteri++)
        {
          w[(j*n_dim)+iteri]=shrink[iteri];
        }
        fw[j]=distance_shrink;
        // if(distance_shrink<fw[n_dim])
        // {
        //   insertionFancy(fw,distance_shrink,shrink,w);
        //   break;
        // }

      }//endfor
      BottomUpMergeSort(fw,n_dimsum,w,n_dim);
    }//endelse
    // }//endif

  }//endfor

  // if(n_dim==10)
  // {
  //   std::vector< unsigned char > frameImage;
  //   float tempVec[7];
  //   tempVec[0]=w[0];
  //   tempVec[1]=w[1];
  //   tempVec[2]=w[2];
  //
  //   tempVec[3]=w[3];
  //   tempVec[4]=w[4];
  //   tempVec[5]=w[5];
  //
  //   tempVec[6]=w[6];
  //   calculateZ(tempVec,rotMat);
  //
  //   reshape(width,height,P,tempVec[6]);
  //
  //   Mat temp;
  //   if(n_dim==10)
  //   {
  //     int tx=w[8]*(width/10);
  //     int ty=w[9]*(height/10);
  //     Mat translation_matrix = (Mat_<float>(2,3) << 1, 0, tx,
  //                                                   0, 1, ty);
  //     warpAffine(imageGray,temp, translation_matrix,
  //               Size(width,height),INTER_LINEAR, BORDER_CONSTANT);
  //
  //     float SCALEFACTOR=w[7];
  //     float SCALEFACTORx=(ORIGINALSCALEx/(P[0][0]))*SCALEFACTOR;
  //     float SCALEFACTORy=(ORIGINALSCALEy/(P[1][1]))*SCALEFACTOR;
  //     S = glm::mat4(1);
  //     S = glm::scale(S, glm::vec3(SCALEFACTORx,SCALEFACTORy,(SCALEFACTORx+SCALEFACTORy)/2 ));//Scale
  //   }
  //   else
  //   {
  //     temp=imageGray;
  //   }
  //
  //   M=T*rotMat*S;
  //   render(
  //           myFrameBuffer,
  //           simpleShader,
  //           ourModel,
  //           M,
  //           V,
  //           P,
  //           width,
  //           height,
  //           frameImage
  //         );
  //
  //   double distance=Loss(temp, frameImage, favg,cff);
  //   cout<< "distance="<<distance<<endl;
  // }

  // cout << " fw[0]="<<fw[0] << " fw[1]="<<fw[1] << " fw[2]="<<fw[2] << " fw[3]="<<fw[3] << " fw[4]="<<fw[4] << " fw[5]="<<fw[5] << " fw[6]="<<fw[6] << " fw[7]="<<fw[7] << endl;
  // cout << "fw[0]="<<fw[0]<<" w[0]="<< w[0]<< " w[1]="<< w[1]<< " w[2]="<< w[2]<< " w[3]="<< w[3]<< " w[4]="<< w[4]<< " w[5]="<< w[5]<< " w[6]="<< w[6] << endl;
  //
  // tempVec[0]=w[0];
  // tempVec[1]=w[1];
  // tempVec[2]=w[2];
  //
  // tempVec[3]=w[3];
  // tempVec[4]=w[4];
  // tempVec[5]=w[5];
  //
  // tempVec[6]=w[6];
  // calculateZ(tempVec,rotMat);
  //
  // cout <<endl<<endl<< rotMat[0][0] << " " << rotMat[0][1] <<" " << rotMat[0][2] << " " << rotMat[0][3] << endl;
  // cout << rotMat[1][0] << " " << rotMat[1][1] <<" " << rotMat[1][2] << " " << rotMat[1][3] << endl;
  // cout << rotMat[2][0] << " " << rotMat[2][1] <<" " << rotMat[2][2] << " " << rotMat[2][3] << endl;
  // cout << rotMat[3][0] << " " << rotMat[3][1] <<" " << rotMat[3][2] << " " << rotMat[3][3] << endl;
  //
  // reshape(width,height,P,w[6]);
  // M=T*rotMat*S;
  // render(
  //         myFrameBuffer,
  //         simpleShader,
  //         ourModel,
  //         M,
  //         V,
  //         P,
  //         width,
  //         height,
  //         frameImage
  //       );
  // SaveFrameN(frameImage,width,height,999 );
}
